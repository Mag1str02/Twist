#pragma once

#include <wheels/core/source_location.hpp>
#include <wheels/intrusive/list.hpp>

#include <utility>

namespace twist::rt::sim {

namespace user {

namespace ss {

//////////////////////////////////////////////////////////////////////

enum class Type {
  Global,
  Local,
};

//////////////////////////////////////////////////////////////////////

struct IVar {
  virtual ~IVar() = default;

  // For memory allocator
  virtual size_t Size() const = 0;

  virtual void Ctor(void* addr) = 0;
  virtual void Dtor() = 0;

  virtual const char* Name() const = 0;
  virtual wheels::SourceLocation SourceLoc() const = 0;

  virtual int InitOrder() const = 0;

  // Isolated memory
  virtual void Burn() = 0;
};

//////////////////////////////////////////////////////////////////////

struct ListTag {};

struct VarBase : IVar,
                 wheels::IntrusiveListNode<VarBase, ss::ListTag> {
  //
};

//////////////////////////////////////////////////////////////////////

class Manager {
  using VarList = wheels::IntrusiveList<VarBase, ListTag>;

 public:
  Manager() = default;

  ~Manager() {
    globals_.UnlinkAll();
  }

  Type Register(VarBase* var) {
    if (!IsMainRunning()) {
      globals_.PushBack(var);
      return Type::Global;
    } else {
      locals_.PushBack(var);
      var->Ctor(AllocVar(var->Size()));
      return Type::Local;
    }
  }

  // Before main, in user-space
  void ConstructGlobalVars() {
    if (!std::exchange(globals_init_order_, true)) {
      SortGlobals();
    }

    for (auto it = globals_.begin(); it != globals_.end(); ++it) {
      try {
        it->Ctor(AllocVar(it->Size()));
      } catch (...) {
        AbortOnException(*it);
      }
    }
  }

  // After main, in user-space
  void DestroyVars() {
    // Destroy in reverse order

    {
      // Locals
      for (auto it = locals_.rbegin(); it != locals_.rend(); ++it) {
        it->Dtor();
      }
      locals_.UnlinkAll();
    }

    {
      // Globals
      for (auto it = globals_.rbegin(); it != globals_.rend(); ++it) {
        it->Dtor();
      }
    }

    DeallocVars();
  }

  // Isolated memory
  void BurnVars() {
    // Order does not matter

    for (VarBase* var : globals_) {
      var->Burn();
    }

    {
      // Locals
      for (VarBase* var : locals_) {
        var->Burn();
      }
      locals_.UnlinkAll();
    }
  }

  static Manager& Instance() {
    static Manager instance;
    return instance;
  }

 private:
  static bool InitGlobalBefore(const IVar* lhs, const IVar* rhs) {
    if (lhs->InitOrder() != rhs->InitOrder()) {
      return lhs->InitOrder() < rhs->InitOrder();
    }

    auto lhs_loc = lhs->SourceLoc();
    auto rhs_loc = rhs->SourceLoc();

    if (lhs_loc.File() != rhs_loc.File()) {
      // Arbitrary
      return lhs_loc.File() < rhs_loc.File();
    }
    // Partial order on variables from the same TU
    return lhs_loc.Line() < rhs_loc.Line();
  }

  void SortGlobals() {
    auto less = [](const IVar* lhs, const IVar* rhs) -> bool {
      return Manager::InitGlobalBefore(lhs, rhs);
    };

    globals_.Sort(less);
  }

  bool IsMainRunning() const;
  void AbortOnException(IVar* global);

  void* AllocVar(size_t size);
  void DeallocVars();

 private:
  VarList globals_;
  bool globals_init_order_ = false;

  VarList locals_;
};

}  // namespace ss

}  // namespace user

}  // namespace twist::rt::sim
