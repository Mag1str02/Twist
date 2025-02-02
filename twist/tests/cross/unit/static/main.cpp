#include <twist/cross.hpp>

#include <twist/ed/static/var.hpp>
#include <twist/ed/static/member.hpp>

#include <twist/ed/std/mutex.hpp>

#include <cassert>
#include <vector>

TWISTED_STATIC(int, x);
TWISTED_STATIC(int, y);

TWISTED_STATIC(std::vector<int>, vec);

class Sequencer {
 public:
  size_t Ctor() {
    return ++count_;
  }

  size_t Dtor() {
    return count_--;
  }

  size_t Count() const {
    return count_;
  }

  ~Sequencer() {
    assert(count_ == 0);
  }

 private:
  size_t count_ = 0;
};

TWISTED_STATIC(Sequencer, wseq);

struct Widget {
  Widget() {
    idx_ = wseq->Ctor();

    data_ = new int{17};
  }

  ~Widget() {
    delete data_;

    size_t idx = wseq->Dtor();
    assert(idx == idx_);
  }

  size_t Index() const {
    return idx_;
  }

 private:
  size_t idx_;
  int* data_;
};

TWISTED_STATIC(Widget, w1);
TWISTED_STATIC(Widget, w2);
TWISTED_STATIC(Widget, w3);

void Woo1() {
  TWISTED_STATIC(Widget, w4);

  assert(w4->Index() == 4);
}

void Woo2() {
  TWISTED_STATIC(Widget, w5);

  assert(w5->Index() == 5);
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

  size_t Get() {
    return count_;
  }

 private:
  size_t count_ = 0;
};

class Klass {
 public:
  Klass() {
    counter->Inc();
  }

  ~Klass() {
    counter->Dec();
  }

  void Foo() {
    //
  }

  void Bar() {
    //
  }

  static size_t Count() {
    return counter->Get();
  }

 private:
  TWISTED_STATIC_MEMBER_DECLARE(Counter, counter);
};

TWISTED_STATIC_MEMBER_DEFINE(Klass, Counter, counter);

void Foo() {
  TWISTED_STATIC(Klass, k);
}

void Bar() {
  TWISTED_STATIC(Klass, k);
  k->Foo();
}

TWISTED_STATIC(twist::ed::std::mutex, mu);

int main() {
  twist::cross::Run([] {
    {
      // int

      // operator*
      assert(*x == 0);
      *x += 1;
      assert(*x == 1);

      // operator&
      int* x_ptr = &x;
      *x_ptr = 2;
      assert(*x == 2);
    }

    {
      // Vector

      // operator->
      assert(vec->empty());

      vec->push_back(1);
      vec->push_back(2);
      vec->push_back(3);

      assert(vec->size() == 3);

      {
        // operator*
        std::vector<int> vec_copy = *vec;
        assert(vec_copy.size() == vec->size());
      }

      {
        // operator&
        std::vector<int>* vec_ptr = &vec;
        vec_ptr->push_back(4);
        assert(vec->size() == 4);
      }
    }

    {
      // Initialization order (globals + locals)

      {
        // Globals

        assert(w3->Index() == 3);
        assert(w2->Index() == 2);
        assert(w1->Index() == 1);

        assert(wseq->Count() == 3);
      }

      {
        // Locals

        Woo1();
        assert(wseq->Count() == 4);

        Woo2();
        assert(wseq->Count() == 5);
      }
    }

    {
      // Member

      assert(Klass::Count() == 0);

      {
        Klass k1;
        assert(Klass::Count() == 1);

        Klass k2;
        assert(Klass::Count() == 2);
      }

      assert(Klass::Count() == 0);

      {
        Klass k3;
        assert(Klass::Count() == 1);
      }

      assert(Klass::Count() == 0);
    }

    {
      // Locals

      assert(Klass::Count() == 0);

      {
        Foo();
        assert(Klass::Count() == 1);
        Foo();
        assert(Klass::Count() == 1);
      }

      {
        Bar();
        assert(Klass::Count() == 2);
      }
    }

    {
      // twist::ed::std::mutex
      mu->lock();
      mu->unlock();
    }
  });

  return 0;  // Passed
}
