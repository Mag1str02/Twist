#include <twist/cross.hpp>

#include <twist/ed/static/thread_local/ptr.hpp>
#include <twist/ed/static/thread_local/var.hpp>

#include <twist/ed/std/thread.hpp>
#include <twist/ed/fmt/print.hpp>

#include <fmt/std.h>

struct Context {
  int x;
  int y;
};

// ~ static thread_local Context* ctx;
TWISTED_STATIC_THREAD_LOCAL_PTR(Context, ctx);

class Widget {
 public:
  Widget() {
    twist::ed::fmt::println("Widget::Widget");
    host_ = twist::ed::std::this_thread::get_id();
  }

  auto Host() {
    return host_;
  }

 private:
  twist::ed::std::thread::id host_;
};

void Bar() {
  twist::ed::fmt::println("Thread {}: ctx -> ({}, {})",
                          twist::ed::std::this_thread::get_id(),
                          ctx->x, ctx->y);
}

void Foo() {
  Bar();
}

void Baz() {
  // ~ static thread_local Widget w;
  TWISTED_STATIC_THREAD_LOCAL(Widget, w);

  twist::ed::fmt::println("{}", w->Host());
}

int main() {
  twist::cross::Run([] {
    Context main{1, 2};
    ctx = &main;

    twist::ed::std::thread t([] {
      Context t{3, 4};
      ctx = &t;

      Foo();

      Baz();
      Baz();
    });

    t.join();

    Foo();

    Baz();
    Baz();
  });

  return 0;
}
