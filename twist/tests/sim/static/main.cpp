#include <twist/sim.hpp>

#include <twist/ed/static/var.hpp>

#include <fmt/core.h>

#include <cassert>

static_assert(twist::build::Sim());

TWISTED_STATIC(int, x);

struct Widget {
  int x;
  int y;
};

TWISTED_STATIC(Widget, w);

struct A {
  A() {
    f = 42;
  }

  int f;
};

TWISTED_STATIC(A, a);

struct B {
  B() {
    // Order of initialization
    assert(a->f == 42);
    f = a->f;
  }

  int f;
};

TWISTED_STATIC(B, b);

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

TWISTED_STATIC(Sequencer, gseq);

struct Gadget {
  int d;
  size_t index;

  Gadget() {
    index = gseq->Ctor();
    d = 1;
  }

  ~Gadget() {
    gseq->Dtor();
  }
};

void Foo() {
  TWISTED_STATIC(Gadget, g);
  assert(g->d == 1);
}

void Bar() {
  TWISTED_STATIC(Gadget, g);
  assert(g->d == 1);
}

int main() {
  twist::sim::SimulatorParams params;
  params.memset_malloc = 0xFF;

  {
    // First simulation

    twist::sim::TestSim(params, [] {
      // Zero initialization
      assert(*x == 0);
      assert(w->x == 0);
      assert(w->y == 0);

      assert(a->f == 42);
      assert(b->f == 42);

      *x = 1;
      w->y = 2;
      a->f = 11;
      b->f = 17;

      {
        // Locals

        assert(gseq->Count() == 0);
        Foo();
        // Construct static locals on first enter
        assert(gseq->Count() == 1);
        Foo();
        // Still one gadget
        assert(gseq->Count() == 1);

        Bar();
        assert(gseq->Count() == 2);
        Bar();
        assert(gseq->Count() == 2);
      }
    });
  }

  {
    // Second simulation

    twist::sim::TestSim(params, [] {
      // Isolation
      assert(*x == 0);
      assert(w->x == 0);
      assert(w->y == 0);
      assert(a->f == 42);
      assert(b->f == 42);

      {
        // Locals
        assert(gseq->Count() == 0);

        Bar();
        assert(gseq->Count() == 1);
      }
    });
  }

  return 0;
}
