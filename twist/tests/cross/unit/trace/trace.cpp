#include <twist/cross.hpp>

#include <twist/trace/domain.hpp>
#include <twist/trace/scope.hpp>

#include <twist/trace/attr/uint.hpp>
#include <twist/trace/attr/ptr.hpp>
#include <twist/trace/attr/bool.hpp>
#include <twist/trace/attr/str.hpp>
#include <twist/trace/attr/fmt.hpp>
#include <twist/trace/attr/unit.hpp>

#include <twist/trace/fmt.hpp>

#include <fmt/core.h>
#include <fmt/format.h>

#include <string>

#include <wheels/test/framework.hpp>

TEST_SUITE(Trace) {
  SIMPLE_TEST(ScopeDomain) {
    twist::cross::Run([] {
      twist::trace::Domain test{"Test"};

      twist::trace::Scope s0{test};
      ASSERT_TRUE(s0.GetDomain() == &test);

      {
        twist::trace::Scope s1;
        ASSERT_TRUE(s1.GetDomain() == &test);
      }
    });
  }

  void FooBar() {
    twist::trace::Scope foobar;

    std::string name = foobar.GetName();
    ASSERT_EQ(name, "FooBar");
  }

  SIMPLE_TEST(ScopeName) {
    twist::cross::Run([] {
      twist::trace::Scope test{"Test"};

      {
        std::string name = test.GetName();
        ASSERT_EQ(name, "Test");

        FooBar();
      }
    });
  }

  SIMPLE_TEST(NestedScopes) {
    twist::cross::Run([] {
      twist::trace::Scope s0{"s0"};

      {
        twist::trace::Scope s1{"s1"};

        ASSERT_TRUE(s1.GetParent() == &s0);

        {
          twist::trace::Scope s2{"s2"};

          ASSERT_TRUE(s2.GetParent() == &s1);
          ASSERT_TRUE(s2.GetParent()->GetParent() == &s0);
        }
      }
    });
  }

  class Widget {
   public:
    void Foo() {
      twist::trace::Scope foo{dom_};
    }

    void Bar() const {
      twist::trace::Scope bar{dom_};
      ASSERT_TRUE(bar.GetDomain()->GetName() == std::string("Widget"));
    }

   private:
    twist::trace::Domain dom_{"Widget"};
  };

  SIMPLE_TEST(Const) {
    twist::cross::Run([] {
      Widget w;
      w.Foo();
      w.Bar();
    });
  }

  SIMPLE_TEST(Attr) {
    twist::cross::Run([] {
      twist::trace::Scope scope{"AttrValue"};

      uint64_t d = 42;

      twist::trace::attr::Uint uint{"d", 42};
      twist::trace::attr::Ptr ptr{"&d", &d};
      twist::trace::attr::Bool flag{"equals42", d == 42};
      twist::trace::attr::Str str{"comment", "test"};
      twist::trace::attr::Unit tag{"unit"};

      ASSERT_EQ(uint.GetName(), std::string_view("d"));
      ASSERT_EQ(uint.GetValue().GetUint(), 42);

      ASSERT_EQ(ptr.GetName(), std::string_view("&d"));
      ASSERT_EQ(ptr.GetValue().GetPtr(), &d);

      ASSERT_EQ(flag.GetName(), std::string_view("equals42"));
      ASSERT_EQ(flag.GetValue().GetBool(), true);

      ASSERT_EQ(str.GetName(), std::string_view("comment"));
      ASSERT_EQ(str.GetValue().GetStr(), "test");

      ASSERT_EQ(tag.GetName(), std::string_view("unit"));

      scope
          .LinkAttr(uint)
          .LinkAttr(ptr)
          .LinkAttr(flag)
          .LinkAttr(str)
          .LinkAttr(tag);

      scope.Note("Just works");
    });
  }

  SIMPLE_TEST(Fmt) {
    using twist::trace::AttrValue;

    auto uint = AttrValue::Uint(42);
    auto ptr = AttrValue::Ptr((void*)0xffff);
    auto flag = AttrValue::Bool(true);
    auto str = AttrValue::Str("str");
    auto unit = AttrValue::Unit();

    auto fmt_values = fmt::format("{},{},{},{},{}", uint, ptr, flag, str, unit);

    ASSERT_EQ(fmt_values, "42,0xffff,true,\"str\",unit");
  }

  SIMPLE_TEST(FmtAttr) {
    twist::cross::Run([] {
      twist::trace::attr::Fmt attr{"test", "{}, {}, {}", 1, "2", 3};
      ASSERT_EQ(attr.GetValue().GetStr(), "1, 2, 3");
    });
  }
}
