#include <wheels/intrusive/forward_list.hpp>

#include <wheels/test/framework.hpp>

using namespace wheels;

TEST_SUITE(IntrusiveForwardList) {
  struct Item : IntrusiveForwardListNode<Item> {
    std::string data;

    Item(std::string _data) : data(std::move(_data)) {
    }
  };

  SIMPLE_TEST(EmptyList) {
    IntrusiveForwardList<Item> list;

    ASSERT_TRUE(list.IsEmpty());
    ASSERT_EQ(list.Size(), 0);
  }

  SIMPLE_TEST(PushPop) {
    IntrusiveForwardList<Item> list;

    Item hello("Hello");
    Item world("World");

    list.PushBack(&hello);

    ASSERT_FALSE(list.IsEmpty());
    ASSERT_EQ(list.Size(), 1);

    list.PushBack(&world);
    ASSERT_EQ(list.Size(), 2);

    Item* first = list.PopFront();
    ASSERT_EQ(list.Size(), 1);
    ASSERT_EQ(first->data, "Hello");

    Item* second = list.PopFront();
    ASSERT_EQ(list.Size(), 0);
    ASSERT_TRUE(list.IsEmpty());
    ASSERT_EQ(second->data, "World");

    Item* third = list.PopFront();
    ASSERT_EQ(third, nullptr);
  }

  SIMPLE_TEST(PushFront) {
    IntrusiveForwardList<Item> list;

    Item hello("Hello");
    Item world("World");

    list.PushBack(&world);
    list.PushFront(&hello);

    ASSERT_EQ(list.Size(), 2);

    Item* first = list.PopFront();
    ASSERT_EQ(first->data, "Hello");
    Item* second = list.PopFront();
    ASSERT_EQ(second->data, "World");


    ASSERT_TRUE(list.IsEmpty());
  }

  SIMPLE_TEST(Append) {
    IntrusiveForwardList<Item> list1;

    Item a("a");
    Item b("b");
    Item c("c");

    list1.PushBack(&a);
    list1.PushBack(&b);
    list1.PushBack(&c);

    ASSERT_EQ(list1.Size(), 3);

    Item d("d");
    Item e("e");

    IntrusiveForwardList<Item> list2;
    list2.PushBack(&d);
    list2.PushBack(&e);

    ASSERT_EQ(list2.Size(), 2);

    list1.Append(list2);

    ASSERT_EQ(list1.Size(), 5);
    ASSERT_TRUE(list2.IsEmpty());

    std::string concat;
    list1.ForEach([&concat](Item* item) {
      concat += item->data;
    });

    ASSERT_EQ(concat, "abcde");

    while (!list1.IsEmpty()) {
      list1.PopFront();
    }
  }

  SIMPLE_TEST(AppendToEmptyList) {
    IntrusiveForwardList<Item> list1;
    IntrusiveForwardList<Item> list2;

    Item first("first");
    Item second("second");

    list2.PushBack(&first);
    list2.PushBack(&second);

    list1.Append(list2);

    ASSERT_EQ(list1.Size(), 2);

    {
      Item* item = list1.PopFront();
      ASSERT_EQ(item->data, "first");
    }
    {
      Item* item = list1.PopFront();
      ASSERT_EQ(item->data, "second");
    }
  }

  SIMPLE_TEST(AppendEmptyList) {
    IntrusiveForwardList<Item> list1;
    IntrusiveForwardList<Item> empty;

    Item first("first");
    Item second("second");

    list1.PushBack(&first);
    list1.PushBack(&second);

    list1.Append(empty);
    ASSERT_EQ(list1.Size(), 2);

    Item third("third");
    list1.PushBack(&third);

    list1.PopFront();
    list1.PopFront();
    list1.PopFront();
  }

  SIMPLE_TEST(ForEach) {
    IntrusiveForwardList<Item> list;

    Item hello("Hello");
    Item comma(", ");
    Item world("World");

    list.PushBack(&hello);
    list.PushBack(&comma);
    list.PushBack(&world);

    std::string message;
    list.ForEach([&message](Item* item) {
      message += item->data;
    });

    list.PopFront();
    list.PopFront();
    list.PopFront();

    ASSERT_TRUE(list.IsEmpty());

    ASSERT_EQ(message, "Hello, World");
  }

  SIMPLE_TEST(MoveConstructable) {
    IntrusiveForwardList<Item> list1;

    Item hello("Hello");
    Item world("World");

    list1.PushBack(&hello);
    list1.PushBack(&world);

    IntrusiveForwardList<Item> list2(std::move(list1));

    ASSERT_TRUE(list1.IsEmpty());

    ASSERT_EQ(list2.Size(), 2);
    Item* first = list2.PopFront();
    ASSERT_EQ(first->data, "Hello");
    Item* second = list2.PopFront();
    ASSERT_EQ(second->data, "World");
  }

  SIMPLE_TEST(NonEmpty) {
    IntrusiveForwardList<Item> list;

    ASSERT_TRUE(list.IsEmpty());
    ASSERT_FALSE(list.NonEmpty());

    Item hello("Hello");

    list.PushBack(&hello);

    ASSERT_FALSE(list.IsEmpty());
    ASSERT_TRUE(list.NonEmpty());

    list.PopFront();
  }

  struct XTag {};
  struct YTag {};
  struct ZTag {};

  struct TaggedItem
      : IntrusiveForwardListNode<TaggedItem, XTag>,
        IntrusiveForwardListNode<TaggedItem, YTag> {
    int data;

    TaggedItem(int d) : data(d) {
    }
  };

  SIMPLE_TEST(Tags) {
    IntrusiveForwardList<TaggedItem, XTag> x;
    IntrusiveForwardList<TaggedItem, YTag> y;

    IntrusiveForwardList<TaggedItem, ZTag> z;

    TaggedItem item{11};

    x.PushBack(&item);
    y.PushBack(&item);

    ASSERT_TRUE(x.NonEmpty());
    ASSERT_TRUE(y.NonEmpty());

    {
      TaggedItem* i = x.PopFront();
      ASSERT_EQ(i->data, 11);
    }

    {
      ASSERT_TRUE(y.NonEmpty());
      TaggedItem* i = y.PopFront();
      ASSERT_EQ(i->data, 11);
    }

    // z.PushBack(&item);  // Does not compile
  }
}
