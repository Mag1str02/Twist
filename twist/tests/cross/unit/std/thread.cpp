#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/std/chrono.hpp>

#include <wheels/core/compiler.hpp>

#include <chrono>

#include <fmt/core.h>
#include <fmt/std.h>

#include <sstream>

#include <unordered_set>

using twist::ed::std::atomic;
using twist::ed::std::thread;
using twist::ed::std::chrono::system_clock;

namespace this_thread = twist::ed::std::this_thread;

using namespace std::chrono_literals;

TEST_SUITE(StdThread) {
  SIMPLE_TEST(Join) {
    twist::cross::Run([] {
      int x = 1;

      thread t([&] {
        x = 2;
      });

      ASSERT_TRUE(t.joinable());

      t.join();

      ASSERT_FALSE(t.joinable());

      ASSERT_EQ(x, 2);
    });
  }

  SIMPLE_TEST(Yield) {
    twist::cross::Run([] {
      bool ok = false;

      thread t([&] {
        for (size_t i = 0; i < 7; ++i) {
          this_thread::yield();  // Just works
        }
        ok = true;
      });

      t.join();
      ASSERT_TRUE(ok);
    });
  }

  SIMPLE_TEST(SleepFor1) {
    twist::cross::Run([] {
      auto start = system_clock::now();

      this_thread::sleep_for(1s);
      this_thread::sleep_for(1s);

      auto elapsed = system_clock::now() - start;
      ASSERT_GE(elapsed, 1s);
    });
  }

  SIMPLE_TEST(SleepFor2) {
    twist::cross::Run([] {
      bool ok = false;

      thread t([&] {
        this_thread::sleep_for(1s);
        ok = true;
      });

      t.join();
      ASSERT_TRUE(ok);
    });
  }

  SIMPLE_TEST(SleepForZero) {
    twist::cross::Run([] {
      this_thread::sleep_for(0us);

      // Almost zero
      this_thread::sleep_for(1us);
    });
  }

  SIMPLE_TEST(SleepForNs) {
    twist::cross::Run([] {
      this_thread::sleep_for(1ns);
    });
  }

  SIMPLE_TEST(MoveCtor) {
    twist::cross::Run([] {
      int x = 1;

      thread t1([&] {
        this_thread::sleep_for(1s);
        x = 2;
      });

      // Move ctor
      thread t2 = std::move(t1);

      ASSERT_FALSE(t1.joinable());
      ASSERT_TRUE(t2.joinable());

      t2.join();

      ASSERT_EQ(x, 2);
    });
  }

  SIMPLE_TEST(DefaultCtor) {
    twist::cross::Run([] {
      thread t;
      ASSERT_FALSE(t.joinable());
    });
  }

  SIMPLE_TEST(MoveAssignment) {
    twist::cross::Run([] {
      thread t1{};

      ASSERT_FALSE(t1.joinable());

      thread t2([] {
      });

      ASSERT_TRUE(t2.joinable());

      // Move assignment
      t1 = std::move(t2);

      ASSERT_TRUE(t1.joinable());
      ASSERT_FALSE(t2.joinable());

      t1.join();
    });
  }

  SIMPLE_TEST(Detach) {
    twist::cross::Run([] {
      atomic<bool> f{false};

      thread t([&f] {
        this_thread::sleep_for(1s);
        f.store(true);
      });

      t.detach();

      ASSERT_FALSE(t.joinable());

      while (!f.load()) {
        this_thread::yield();
      }
    });
  }

  SIMPLE_TEST(SleepUntil) {
    twist::cross::Run([] {
      auto future = system_clock::now() + 1s;
      this_thread::sleep_until(future);
    });
  }

  SIMPLE_TEST(GetId) {
    twist::cross::Run([] {
      thread::id main_id = this_thread::get_id();

      {
        // Copyable
        thread::id copy = main_id;
        WHEELS_UNUSED(copy);
      }

      thread t([main_id] {
        thread::id t_id = this_thread::get_id();

        // Different
        ASSERT_NE(main_id, t_id);
      });

      {
        // Persistent
        thread::id main_id2 = this_thread::get_id();
        ASSERT_EQ(main_id, main_id2);
      }

      t.join();
    });
  }

  SIMPLE_TEST(ImpossibleId) {
    thread::id impossible{};
  }

  SIMPLE_TEST(PrintId) {
    // TODO: Disable determinism check
    twist::cross::Run([] {
      thread::id my_id = this_thread::get_id();

      // ostream
      std::stringstream out;
      out << my_id;
      auto id_str1 = out.str();

      // fmt
      auto id_str2 = fmt::format("{}", my_id);

      ASSERT_EQ(id_str1, id_str2);
    });
  }

  SIMPLE_TEST(CompareIds) {
    twist::cross::Run([] {
      thread::id i_id{};
      thread::id my_id = this_thread::get_id();

      {
        ASSERT_TRUE(my_id == my_id);
        ASSERT_FALSE(my_id != my_id);
        ASSERT_TRUE(my_id <= my_id);
        ASSERT_TRUE(my_id >= my_id);
        ASSERT_FALSE(my_id < my_id);
        ASSERT_FALSE(my_id > my_id);
      }

      {
        ASSERT_FALSE(my_id == i_id);
        ASSERT_TRUE(my_id != i_id);
      }

      thread t([main_id = my_id] {
        thread::id t_id = this_thread::get_id();

        ASSERT_FALSE(t_id == main_id);
        ASSERT_TRUE(t_id != main_id);
        ASSERT_TRUE((t_id < main_id) != (t_id > main_id));
        ASSERT_TRUE((t_id <= main_id) != (t_id >= main_id));
      });

      t.join();
    });
  }

  SIMPLE_TEST(HashId) {
    twist::cross::Run([] {
      std::unordered_set<thread::id> ids;
      ids.insert(this_thread::get_id());
    });
  }

  SIMPLE_TEST(HardwareConcurrency) {
    twist::cross::Run([] {
      // Just works
      unsigned int hw = thread::hardware_concurrency();
      WHEELS_UNUSED(hw);
    });
  }

  SIMPLE_TEST(NativeHandle) {
    twist::cross::Run([] {
      thread t([] {});
      // Just works
      thread::native_handle_type h = t.native_handle();
      WHEELS_UNUSED(h);

      t.join();
    });
  }

  SIMPLE_TEST(Swap) {
    twist::cross::Run([] {
      bool f = false;

      thread t1([&f] {
        this_thread::sleep_for(1s);
        f = true;
      });

      thread t2{};

      t2.swap(t1);

      ASSERT_FALSE(t1.joinable());
      ASSERT_TRUE(t2.joinable());

      t2.join();

      ASSERT_TRUE(f);
    });
  }

  SIMPLE_TEST(RoutineArguments) {
    twist::cross::Run([] {
      {
        thread t(
            [](int x) {
              ASSERT_EQ(x, 1);
            },
            1
        );

        t.join();
      }

      {
        thread t(
            [](int x, int y, int z) {
              ASSERT_EQ(x, 1);
              ASSERT_EQ(y, 2);
              ASSERT_EQ(z, 3);
            },
            1, 2, 3
        );

        t.join();
      }

      {
        thread t(
            [](std::vector<int> v) {
              ASSERT_EQ(v.size(), 3);
              ASSERT_EQ(v[0], 0);
              ASSERT_EQ(v[1], 1);
              ASSERT_EQ(v[2], 2);
            },
            std::vector<int>{0, 1, 2}
        );

        t.join();
      }

      {
        int x = 0;

        thread t(
            [](int& x) {
              x = 1;
            },
            std::ref(x)
        );

        t.join();

        ASSERT_EQ(x, 1);
      }

      {
        struct A {};
        struct B {};
        struct C {};
        struct D {};
        struct E {};

        thread t1([](A) {}, A{});
        t1.join();

        thread t2([](A, B) {}, A{}, B{});
        t2.join();

        thread t3([](A, B, C) {}, A{}, B{}, C{});
        t3.join();

        thread t4([](A, B, C, D) {}, A{}, B{}, C{}, D{});
        t4.join();

        thread t5([](A, B, C, D, E) {}, A{}, B{}, C{}, D{}, E{});
        t5.join();
      }
    });
  }

  SIMPLE_TEST(ClosureDtor) {
    twist::cross::Run([] {
      struct State {
        //
      };

      struct Future {
        std::shared_ptr<State> state;
      };

      struct Promise {
        Promise()
            : state{std::make_shared<State>()} {
          //
        }

        Promise(Promise&&) = default;

        ~Promise() {
          //
        };

        Future MakeFuture() {
          return Future{state};
        }

        std::shared_ptr<State> state;
      };

      Promise p;
      auto f = p.MakeFuture();

      twist::ed::std::thread t([p = std::move(p)] {
        //
      });

      twist::ed::std::this_thread::sleep_for(1s);

      // Implementation detail
      ASSERT_TRUE(f.state.use_count() == 1);

      t.join();
    });
  }
}
