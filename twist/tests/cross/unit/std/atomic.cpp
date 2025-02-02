#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/std/atomic.hpp>

#include <wheels/core/compiler.hpp>

#include <array>

using twist::ed::std::atomic;
using twist::ed::std::atomic_flag;

TEST_SUITE(StdAtomicIntegral) {
  SIMPLE_TEST(CtorLoad) {
    twist::cross::Run([] {
      atomic<int> x = 0;
      ASSERT_EQ(x.load(), 0);

      atomic<int> y{1};
      ASSERT_EQ(y.load(), 1);
    });
  }

  SIMPLE_TEST(Store) {
    twist::cross::Run([] {
      atomic<int> a{0};

      a.store(1);
      ASSERT_EQ(a.load(), 1);

      a.store(2);
      ASSERT_EQ(a.load(), 2);
    });
  }

  SIMPLE_TEST(Exchange) {
    twist::cross::Run([] {
      atomic<int> a{0};

      {
        int r = a.exchange(1);
        ASSERT_EQ(r, 0);
        ASSERT_EQ(a.load(), 1);
      }

      {
        int r = a.exchange(2);
        ASSERT_EQ(r, 1);
        ASSERT_EQ(a.load(), 2);
      }
    });
  }

  SIMPLE_TEST(FetchAnd) {
    twist::cross::Run([] {
      atomic<int> a{1};

      {
        int r = a.fetch_add(3);
        ASSERT_EQ(r, 1);
        ASSERT_EQ(a.load(), 4);
      }

      {
        int r = a.fetch_sub(2);
        ASSERT_EQ(r, 4);
        ASSERT_EQ(a.load(), 2);
      }

      {
        int r = a.fetch_or(1);
        ASSERT_EQ(r, 2);
        ASSERT_EQ(a.load(), 2 | 1);
      }

      {
        int r = a.fetch_and(2);
        ASSERT_EQ(r, 3);
        ASSERT_EQ(a.load(), 3 & 2);
      }

      {
        int r = a.fetch_xor(7);
        ASSERT_EQ(r, 2);
        ASSERT_EQ(a.load(), 2 ^ 7);
      }
    });
  }

  SIMPLE_TEST(CompareExchangeStrong) {
    twist::cross::Run([] {
      atomic<int> a{3};

      {
        // Success

        int expected{3};
        bool ok = a.compare_exchange_strong(expected, 7);

        ASSERT_TRUE(ok);
        ASSERT_EQ(a.load(), 7);  // New
      }

      {
        // Failure

        int expected{4};
        bool ok = a.compare_exchange_strong(expected, 5);

        ASSERT_FALSE(ok);
        ASSERT_EQ(a.load(), 7);  // Still
        ASSERT_EQ(expected, 7);  // Read
      }
    });
  }

  SIMPLE_TEST(CompareExchangeWeak) {
    twist::cross::Run([] {
      atomic<int> a{3};

      {
        // Success

        int expected{3};
        while (!a.compare_exchange_weak(expected, 7)) {
          // Spurious failure, try again
        }

        ASSERT_EQ(a.load(), 7);  // New
      }

      {
        // Failure

        for (size_t i = 0; i < 128; ++i) {
          int expected{4};
          bool ok = a.compare_exchange_weak(expected, 5);

          ASSERT_FALSE(ok);
          ASSERT_EQ(a.load(), 7);  // Still
        }
      }
    });
  }

  SIMPLE_TEST(Operators) {
    twist::cross::Run([] {
      atomic<int> a{0};

      // store, load

      {
        a = 7;
        int r = a;
        ASSERT_EQ(r, 7);
      }

      a.store(0);  // Reset

      // Increments

      {
        int r = a++;
        ASSERT_EQ(r, 0);
        ASSERT_EQ(a.load(), 1);
      }

      {
        int r = ++a;
        ASSERT_EQ(r, 2);
        ASSERT_EQ(a.load(), 2);
      }

      // Decrements

      {
        int r = --a;
        ASSERT_EQ(r, 1);
        ASSERT_EQ(a.load(), 1);
      }

      {
        int r = a--;
        ASSERT_EQ(r, 1);
        ASSERT_EQ(a.load(), 0);
      }

      // fetch_{op}

      a.store(0);  // Reset

      {
        a += 7;
        ASSERT_EQ(a.load(), 7);
      }

      {
        a -= 3;
        ASSERT_EQ(a.load(), 4);
      }

      {
        a |= 1;
        ASSERT_EQ(a.load(), 5);

        a |= 1;
        ASSERT_EQ(a.load(), 5);
      }

      {
        a &= 3;
        ASSERT_EQ(a.load(), 1);
      }

      {
        a ^= 3;
        ASSERT_EQ(a.load(), 2);
      }
    });
  }

  SIMPLE_TEST(MemoryOrder) {
    twist::cross::Run([] {
      atomic<int> a{0};

      // store / load

      a.store(1, std::memory_order::release);
      a.load(std::memory_order::acquire);

      // fetch_{op}

      a.fetch_add(3, std::memory_order::seq_cst);
      a.fetch_sub(2, std::memory_order::acq_rel);

      a.fetch_and(1, std::memory_order::relaxed);
      a.fetch_or(2, std::memory_order::release);
      a.fetch_xor(3, std::memory_order::seq_cst);

      // compare_exchange_{weak, strong}

      {
        int e = 1;
        a.compare_exchange_strong(e, 5, std::memory_order::release, std::memory_order::relaxed);
      }

      {
        int e = 1;
        a.compare_exchange_weak(e, 5, std::memory_order::release, std::memory_order::relaxed);
      }
    });
  }

  SIMPLE_TEST(IsLockFree) {
    twist::cross::Run([] {
      atomic<int> a{0};

      {
        a.is_lock_free();
      }

      {
        bool always = atomic<int>::is_always_lock_free;
        WHEELS_UNUSED(always);
      }
    });
  }

#if defined(__TWIST_FAULTY__)

  SIMPLE_TEST(DebugLoad) {
    twist::cross::Run([] {
      atomic<int> a{3};

      int r = a.DebugLoad();
      ASSERT_EQ(r, 3);
    });
  }

#endif

}

TEST_SUITE(StdAtomicStruct) {
  struct Test {
    int x;
  };

  SIMPLE_TEST(StoreLoad) {
    twist::cross::Run([] {
      atomic<Test> a{Test{7}};

      {
        auto t = a.load();
        ASSERT_EQ(t.x, 7);
      }

      {
        a.store(Test{3});

        auto t1 = a.load();
        ASSERT_EQ(t1.x, 3);

        auto t2 = a.load();
        ASSERT_EQ(t2.x, 3);
      }
    });
  }

  SIMPLE_TEST(Exchange) {
    twist::cross::Run([] {
      atomic<Test> a{Test{7}};

      {
        auto r = a.exchange(Test{5});
        ASSERT_EQ(r.x, 7);
        ASSERT_EQ(a.load().x, 5);
      }
    });
  }

  SIMPLE_TEST(CompareExchangeStrong) {
    twist::cross::Run([] {
      atomic<Test> a{Test{3}};

      {
        // Success

        Test expected{3};
        bool ok = a.compare_exchange_strong(expected, Test{7});

        ASSERT_TRUE(ok);
        ASSERT_EQ(a.load().x, 7);  // New
      }

      {
        // Failure

        Test expected{4};
        bool ok = a.compare_exchange_strong(expected, Test{5});

        ASSERT_FALSE(ok);
        ASSERT_EQ(a.load().x, 7);  // Still
        ASSERT_EQ(expected.x, 7);  // Read
      }
    });
  }

  SIMPLE_TEST(CompareExchangeWeak) {
    twist::cross::Run([] {
      atomic<Test> a{Test{3}};

      {
        // Success

        Test expected{3};
        while (!a.compare_exchange_weak(expected, Test{7})) {
          // Spurious failure, try again
        }

        ASSERT_EQ(a.load().x, 7);  // New
      }

      {
        // Failure

        for (size_t i = 0; i < 128; ++i) {
          Test expected{4};
          bool ok = a.compare_exchange_weak(expected, Test{5});

          ASSERT_FALSE(ok);
          ASSERT_EQ(a.load().x, 7);  // Still
        }
      }
    });
  }

  SIMPLE_TEST(Operators) {
    twist::cross::Run([] {
      atomic<Test> a{Test{7}};

      a = Test{3};
      Test r = a;

      ASSERT_EQ(r.x, 3);
    });
  }

  // To be continued
}

TEST_SUITE(StdAtomicPtr) {
  SIMPLE_TEST(StoreLoad) {
    twist::cross::Run([] {
      int x = 7;
      int y = 9;
      atomic<int*> a{&x};

      {
        int* p = a.load();
        ASSERT_EQ(p, &x);
      }

      {
        a.store(&y);
        int* p = a.load();
        ASSERT_EQ(p, &y);
      }
    });
  }

  struct Node {
    Node* next = nullptr;
  };

  SIMPLE_TEST(Exchange) {
    twist::cross::Run([] {
      atomic<Node*> a{nullptr};

      Node n1;
      Node n2;

      {
        Node* r = a.exchange(&n1);
        ASSERT_EQ(r, nullptr);
        ASSERT_EQ(a.load(), &n1);
      }

      {
        Node* r = a.exchange(&n2);
        ASSERT_EQ(r, &n1);
        ASSERT_EQ(a.load(), &n2);
      }
    });
  }

  SIMPLE_TEST(CompareExchange) {
    twist::cross::Run([] {
      atomic<Node*> top{nullptr};

      Node n1;
      Node n2;

      {
        // Strong

        n1.next = top.load();
        bool ok = top.compare_exchange_strong(n1.next, &n1);

        ASSERT_TRUE(ok);
        ASSERT_EQ(top.load(), &n1);
      }

      {
        // Weak

        n2.next = top.load();
        while (!top.compare_exchange_weak(n2.next, &n2)) {
          // Spurious failure, try again
        }
        ASSERT_EQ(top.load(), &n2);
      }
    });
  }

  SIMPLE_TEST(Operators) {
    twist::cross::Run([] {
      atomic<Node*> a{nullptr};

      Node n;

      a = &n;

      Node* r = a;
      ASSERT_EQ(r, &n);
    });
  }

  SIMPLE_TEST(Arithmetic) {
    twist::cross::Run([] {
      struct Widget {
        int d;
      };

      std::array<Widget, 5> arr{Widget{0}};

      atomic<Widget*> ptr = &arr[0];

      {
        Widget* p = ++ptr;
        ASSERT_EQ(p, &arr[1]);
        ASSERT_EQ(ptr.load(), &arr[1]);
      }

      {
        Widget* p = ptr++;
        ASSERT_EQ(p, &arr[1]);
        ASSERT_EQ(ptr.load(), &arr[2]);
      }

      {
        ptr += 2;
        ASSERT_EQ(ptr.load(), &arr[4]);
      }

      {
        ptr -= 3;
        ASSERT_EQ(ptr.load(), &arr[1]);
      }


      ptr.store(&arr[3]);

      {
        Widget* p = ptr--;
        ASSERT_EQ(p, &arr[3]);
        ASSERT_EQ(ptr.load(), &arr[2]);
      }

      {
        Widget* p = --ptr;
        ASSERT_EQ(p, &arr[1]);
        ASSERT_EQ(ptr.load(), &arr[1]);
      }
    });
  }

#if defined(__TWIST_FAULTY__)

  SIMPLE_TEST(DebugLoad) {
    twist::cross::Run([] {
      Node n;
      atomic<Node*> a{&n};

      Node* r = a.DebugLoad();
      ASSERT_EQ(r, &n);
    });
  }

#endif

  // To be continued
}

TEST_SUITE(StdAtomicFlag) {
  SIMPLE_TEST(Works) {
    twist::cross::Run([] {
      atomic_flag f;

      ASSERT_FALSE(f.test());

      {
        bool r = f.test_and_set();
        ASSERT_FALSE(r);
        ASSERT_TRUE(f.test());
      }

      f.clear();
      ASSERT_FALSE(f.test());
    });
  }

  SIMPLE_TEST(MemoryOrder) {
    twist::cross::Run([] {
      atomic_flag f;

      ASSERT_FALSE(f.test(std::memory_order::seq_cst));

      {
        bool r = f.test_and_set(std::memory_order::acq_rel);
        ASSERT_FALSE(r);
        ASSERT_TRUE(f.test(std::memory_order::acquire));
      }

      f.clear(std::memory_order::release);
      ASSERT_FALSE(f.test());
    });
  }

#if defined(__TWIST_FAULTY__)

  SIMPLE_TEST(DebugTest) {
    twist::cross::Run([] {
      atomic_flag f;

      ASSERT_FALSE(f.DebugTest());

      f.test_and_set();

      ASSERT_TRUE(f.DebugTest());
    });
  }

#endif

}
