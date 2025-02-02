#pragma once

#include <twist/rt/sim/user/syscall/spawn.hpp>

#include <twist/rt/sim/user/library/std_like/atomic.hpp>

#include <wheels/core/compiler.hpp>
#include <wheels/core/source_location.hpp>

#include <twist/trace/domain.hpp>

#include <type_traits>

namespace twist::rt::sim {

namespace user::test {

class WaitGroup {
 private:
  struct UserStateNode : system::IThreadUserState {
    UserStateNode* next;
  };

  template <typename F>
  struct UserState : UserStateNode {
    explicit UserState(WaitGroup* w, size_t i, F&& f)
        : wg(w), index(i), user(std::move(f)) {
    }

    wheels::SourceLocation SourceLoc() const noexcept override {
      return wg->ctor_source_loc_;
    }

    void RunUser() override {
      if constexpr (std::is_invocable_v<F, size_t>) {
        // Destroy user state at the end of RunUser
        auto u = std::move(user);
        u(index);
      } else {
        WHEELS_UNUSED(index);
        auto u = std::move(user);
        u();
      }
    }

    void AtThreadExit() noexcept override {
      wg->AtThreadExit();
      delete this;
    }

    WaitGroup* wg;
    size_t index;
    F user;
  };

 public:
  WaitGroup(wheels::SourceLocation ctor = wheels::SourceLocation::Current());

  template <typename F>
  WaitGroup& Add(F fn) {
    UserStateNode* r = new UserState{this, count_, std::move(fn)};

    r->next = head_;
    head_ = r;

    ++count_;

    return *this;
  }

  template <typename F>
  WaitGroup& Add(size_t count, F fn) {
    for (size_t i = 0; i < count; ++i) {
      Add(fn);
    }

    return *this;
  }

  void Join(wheels::SourceLocation call_site = wheels::SourceLocation::Current());

 private:
  void AtThreadExit() noexcept;

 private:
  wheels::SourceLocation ctor_source_loc_;

  UserStateNode* head_ = nullptr;
  size_t count_{0};
  library::std_like::atomic<uint64_t> left_{0};
  library::std_like::atomic<uint32_t> done_{0};

  trace::Domain wg_{"test::WaitGroup"};
};

}  // namespace user::test

}  // namespace twist::rt::sim
