#include <twist/cross.hpp>

#include <twist/ed/static/thread_local/ptr.hpp>
#include <twist/ed/static/thread_local/var.hpp>
#include <twist/ed/static/thread_local/member.hpp>
#include <twist/ed/static/member.hpp>

#include <twist/ed/std/thread.hpp>

#include <cassert>

using twist::ed::std::thread;

auto ThisThreadId() {
  return twist::ed::std::this_thread::get_id();
}

class Counter {
 public:
  ~Counter() {
    assert(count_ == 0);
  }

  void Inc() {
    ++count_;
  }

  void Dec() {
    --count_;
  }

  size_t Count() const {
    return count_;
  }

 private:
  size_t count_ = 0;
};

class Host {
 public:
  Host() {
    id_ = ThisThreadId();
  }

  auto Id() const {
    return id_;
  }

 private:
  twist::ed::std::thread::id id_;
};

class Widget {
 public:
  Widget() {
    counter->Inc();
  }

  ~Widget() {
    counter->Dec();
  }

  auto Host() const {
    return host_.Id();
  }

  void Foo() {
  }

  static size_t Count() {
    return counter->Count();
  }

 private:
  class Host host_;
  TWISTED_STATIC_MEMBER_DECLARE(Counter, counter);
};

TWISTED_STATIC_MEMBER_DEFINE(Widget, Counter, counter);

TWISTED_STATIC_THREAD_LOCAL(Widget, w);

struct Fiber {
  int id;
};

TWISTED_STATIC_THREAD_LOCAL_PTR(Fiber, f);

struct Gadget {
 public:
  Gadget() {
    counter->Inc();
  }

  ~Gadget() {
    counter->Dec();
  }

  auto Host() const {
    return host_.Id();
  }

  static size_t Count() {
    return counter->Count();
  }

 private:
  class Host host_;
  TWISTED_STATIC_MEMBER_DECLARE(Counter, counter);
};

TWISTED_STATIC_MEMBER_DEFINE(Gadget, Counter, counter);

Gadget* Foo() {
  TWISTED_STATIC_THREAD_LOCAL(Gadget, g);

  assert(Gadget::Count() > 0);

  return &g;
}

class Member {
 public:
  Member() {
    counter->Inc();
  }

  ~Member() {
    counter->Dec();
  }

  auto Host() {
    return host_.Id();
  }

  void Foo() {
  }

  static size_t Count() {
    return counter->Count();
  }

 private:
  class Host host_;
  TWISTED_STATIC_MEMBER_DECLARE(Counter, counter);
};

TWISTED_STATIC_MEMBER_DEFINE(Member, Counter, counter);

class Klass {
 public:
  static void Foo() {
    m->Foo();
  }

  static Member* ThisThreadMember() {
    return &m;
  }

 private:
  TWISTED_STATIC_THREAD_LOCAL_MEMBER_DECLARE(Member, m);
};

TWISTED_STATIC_THREAD_LOCAL_MEMBER_DEFINE(Klass, Member, m);

struct Representative {
  Representative() {
    thread_id = twist::ed::std::this_thread::get_id();
  }

  twist::ed::std::thread::id thread_id;
};

TWISTED_STATIC_THREAD_LOCAL(Representative, repr);

TWISTED_STATIC_THREAD_LOCAL_PTR_INIT(Representative, my_repr, &repr);

int main() {
  twist::cross::Run([] {
    auto t1_id = ThisThreadId();

    {
      // Global Widget

      assert(Widget::Count() == 0);

      {
        thread t_some([] {});
        t_some.join();

        assert(Widget::Count() == 0);
      }

      assert(w->Host() == t1_id);
      (*w).Foo();  // operator*
      w->Foo();  // operator->
      assert(Widget::Count() == 1);

      Widget* w1 = &w;  // operator&
      assert(w1->Host() == t1_id);

      {
        thread t2([&] {
          auto t2_id = ThisThreadId();

          assert(Widget::Count() == 1);

          w->Foo();

          assert(Widget::Count() == 2);

          (*w).Foo();
          assert(w->Host() == t2_id);

          Widget* w2 = &w;

          assert(w1 != w2);

          assert(w1->Host() == t1_id);
          assert(w2->Host() == t2_id);
        });

        t2.join();
      }

      assert(Widget::Count() == 1);
    }

    {
      // Local Gadget

      assert(Gadget::Count() == 0);

      {
        thread t_some([] {});
        t_some.join();

        assert(Gadget::Count() == 0);
      }

      Gadget* g1 = Foo();

      assert(Gadget::Count() == 1);
      assert(g1->Host() == t1_id);

      {
        thread t2([&] {
          auto t2_id = ThisThreadId();

          assert(Gadget::Count() == 1);

          Gadget* g2 = Foo();

          assert(Gadget::Count() == 2);

          assert(g1 != g2);
          assert(g2->Host() == t2_id);
          assert(g1->Host() == t1_id);
        });

        t2.join();
      }

      // t2's Gadget destroyed
      assert(Gadget::Count() == 1);

      assert(g1->Host() == t1_id);
    }

    {
      // Pointer

      assert(f == nullptr);

      Fiber f1{1};
      Fiber f3{3};

      f = &f1;

      assert(f == &f1);
      assert(f != &f3);
      assert(f != nullptr);
      assert(f->id == f1.id);
      assert((*f).id == f1.id);

      {
        thread t2([] {
          Fiber f2{2};

          f = &f2;

          assert(f == &f2);
          assert(f->id == f2.id);
          assert((*f).id == f2.id);
        });

        t2.join();
      }

      assert(f->id == f1.id);
    }

    {
      // Member

      // ?
      // assert(Member::Count() == 0);

      Klass::Foo();
      assert(Member::Count() == 1);

      Klass k1;
      Klass k2;
      k1.Foo();
      k2.Foo();

      assert(Klass::ThisThreadMember()->Host() == ThisThreadId());

      {
        thread t([] {
          Klass::Foo();
          assert(Member::Count() == 2);
          Klass::Foo();
          Klass k3;
          k3.Foo();
          assert(Member::Count() == 2);

          assert(Klass::ThisThreadMember()->Host() == ThisThreadId());
        });

        t.join();

        assert(Member::Count() == 1);
      }
    }

    {
      auto id = my_repr->thread_id;
      assert(id == twist::ed::std::this_thread::get_id());

      twist::ed::std::thread t2([] {
        auto id = my_repr->thread_id;
        assert(id == twist::ed::std::this_thread::get_id());
      });

      t2.join();
    }
  });

  return 0;  // Passed
}
