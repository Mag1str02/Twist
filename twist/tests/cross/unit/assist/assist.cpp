#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/assist/memory.hpp>
#include <twist/assist/shared.hpp>
#include <twist/assist/preempt.hpp>

#include <cassert>

TEST_SUITE(Memory) {
  struct Widget {
    void Foo() {
    }
  };

  SIMPLE_TEST(New) {
    twist::cross::Run([] {
      Widget* w = twist::assist::New<Widget>();
      delete w;
    });
  }

  SIMPLE_TEST(AccessObject) {
    twist::cross::Run([] {
      Widget* w = new Widget{};
      twist::assist::MemoryAccess(w, sizeof(Widget));
      delete w;
    });
  }

  SIMPLE_TEST(AccessBuffer) {
    twist::cross::Run([] {
      char* buf = new char[128];

      twist::assist::MemoryAccess(buf, 128);
      twist::assist::MemoryAccess(buf + 64, 16);

      delete[] buf;
    });
  }

  SIMPLE_TEST(Ptr) {
    twist::cross::Run([] {
      twist::assist::Ptr<Widget> w1 = new Widget{};
      w1->Foo();    // operator ->
      w1()->Foo();   // capture source location
      (*w1).Foo();  // operator ==

      {
        assert(w1);  // operator bool

        twist::assist::Ptr<Widget> w_nullptr = nullptr;
        assert(!w_nullptr);
      }

      Widget* w2 = w1;  // operator T*

      Widget* w3 = new Widget{};

      twist::assist::Ptr<Widget> w4 = new Widget{};

      // ==
      assert(w1 == w1);  // Ptr<T> == Ptr<T>
      assert(w1 == w2);  // Ptr<T> == T*
      assert(w2 == w1);  // T* == Ptr<T>

      // !=
      assert(w1 != w4);  // Ptr<T> != Ptr<T>
      assert(w1 != w3);  // Ptr<T> != T*
      assert(w3 != w1);  // T* != Ptr<T>

      // operator&
      Widget** wp = &w1;
      (*wp)->Foo();

      delete w1.raw;
      // delete w2;  // w2 == w1
      delete w3;
      delete w4.raw;
    });
  }
}

TEST_SUITE(Shared) {
  SIMPLE_TEST(ReadWrite) {
    twist::cross::Run([] {
      twist::assist::Shared<int> var{0};

      ASSERT_TRUE(0 == var);
      *var = 1;
      ASSERT_TRUE(1 == var);

      var.Write(2);
      ASSERT_TRUE(var.Read() == 2);
    });
  }

  class Job {
   public:
    Job() : done(false) {
    }

    void DoExplicit() {
      done.Write(true);
    }

    void DoOperator() {
      *done = true;
    }

    bool DoneExplicit() const {
      return done.Read();
    }

    bool DoneCast() const {
      return done;
    }

   private:
    twist::assist::Shared<bool> done;
  };

  SIMPLE_TEST(Job) {
    twist::cross::Run([] {
      {
        Job job1;

        ASSERT_FALSE(job1.DoneExplicit());
        ASSERT_FALSE(job1.DoneCast());

        job1.DoExplicit();

        ASSERT_TRUE(job1.DoneExplicit());
        ASSERT_TRUE(job1.DoneCast());

      }

      {
        Job job2;

        ASSERT_FALSE(job2.DoneExplicit());
        ASSERT_FALSE(job2.DoneCast());

        job2.DoOperator();

        ASSERT_TRUE(job2.DoneExplicit());
        ASSERT_TRUE(job2.DoneCast());

      }
    });
  }

  SIMPLE_TEST(Views) {
    // TODO
  }
}

TEST_SUITE(Preemption) {
  SIMPLE_TEST(JustWorks) {
    twist::cross::Run([] {
      twist::assist::PreemptionPoint();

      {
        twist::assist::NoPreemptionGuard guard;
        //
      }
    });
  }
}
