#include <twist/cross.hpp>

#include <twist/ed/static/var.hpp>
#include <twist/ed/static/member.hpp>

#include <twist/ed/fmt/print.hpp>

#include <fmt/format.h>

using twist::ed::fmt::println;

struct Widget {
  struct Buf {};
 public:
  Widget() {
    println("[{}] Widget::Widget", fmt::ptr(this));

    buf_ = new Buf{};
  }

  ~Widget() {
    println("[{}] Widget::~Widget", fmt::ptr(this));
    delete buf_;
  }

  void Foo() {
    println("[{}] Widget::Foo", fmt::ptr(this));
  }

  void Bar() {
    println("[{}] Widget::Bar", fmt::ptr(this));
  }

 private:
  Buf* buf_;
};

struct Gadget {
  Gadget() {
    println("Gadget::Gadget");
  }

  ~Gadget() {
    println("Gadget::~Gadget");
  }
};

void Baz() {
  // ~ static Widget w2;
  TWISTED_STATIC(Widget, w2);

  w2->Bar();
}

// ~ static Widget w1;
TWISTED_STATIC(Widget, w1);

// ~ static Gadget g;
TWISTED_STATIC(Gadget, g);

class Counter {
 public:
  Counter() {
    println("Counter::Counter");
  }

  ~Counter() {
    println("Counter::~Counter");
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

  static size_t Count() {
    return counter->Get();
  }

 private:
  // ~ static Counter counter;
  TWISTED_STATIC_MEMBER_DECLARE(Counter, counter);
};

// ~ Counter Klass::counter;
TWISTED_STATIC_MEMBER_DEFINE(Klass, Counter, counter);

int main() {
  twist::cross::Run([] {
    println("Start");

    w1->Foo();

    {
      println("-> Baz");
      Baz();
    }

    {
      println("Klass count = {}", Klass::Count());

      {
        Klass k1;
        println("Klass count = {}", Klass::Count());

        Klass k2;
        println("Klass count = {}", Klass::Count());
      }

      println("Klass count = {}", Klass::Count());
    }

    println("Exit");
  });

  return 0;
}
