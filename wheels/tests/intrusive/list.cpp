#include <wheels/intrusive/list.hpp>

#include <wheels/test/framework.hpp>

using namespace wheels;

TEST_SUITE(IntrusiveList) {
  struct Item : IntrusiveListNode<Item> {
    std::string data;

    Item(std::string _data) : data(std::move(_data)) {
    }
  };

  SIMPLE_TEST(PushThenPop) {
    Item first("hello");
    Item second("world");
    Item third("!");

    IntrusiveList<Item> items;

    ASSERT_TRUE(items.IsEmpty());

    items.PushBack(&first);
    items.PushBack(&second);
    items.PushBack(&third);

    ASSERT_FALSE(items.IsEmpty());

    Item* pop_first = items.PopFront();
    ASSERT_EQ(pop_first->data, "hello");

    Item* pop_second = items.PopFront();
    ASSERT_EQ(pop_second->data, "world");

    Item* pop_third = items.PopFront();
    ASSERT_EQ(pop_third->data, "!");

    ASSERT_TRUE(items.IsEmpty());

    ASSERT_EQ(items.PopFront(), nullptr);
  }

  SIMPLE_TEST(FrontBack) {
    IntrusiveList<Item> items;

    Item a("a");
    Item b("b");
    Item c("c");
    Item d("d");

    items.PushBack(&a);
    items.PushFront(&b);
    items.PushBack(&c);
    items.PushFront(&d);

    Item* item;

    item = items.PopFront();
    ASSERT_EQ(item, &d);
    item = items.PopBack();
    ASSERT_EQ(item, &c);
    item = items.PopFront();
    ASSERT_EQ(item, &b);
    item = items.PopFront();
    ASSERT_EQ(item, &a);
  }

  SIMPLE_TEST(Iterator) {
    Item first("hello");
    Item second("world");
    Item third("!");

    IntrusiveList<Item> items;

    ASSERT_EQ(items.begin(), items.end());

    items.PushBack(&first);
    items.PushBack(&second);
    items.PushBack(&third);

    auto iterator = items.begin();
    auto end = items.end();

    ASSERT_NE(iterator, end);
    ASSERT_EQ((*iterator)->data, "hello");

    ++iterator;
    ASSERT_NE(iterator, end);

    ++iterator;
    ++iterator;
    ASSERT_EQ(iterator, end);

    items.UnlinkAll();
  }

  SIMPLE_TEST(RangeBasedForLoop) {
    Item first("hello");
    Item second("world");
    Item third("!");

    IntrusiveList<Item> items;
    items.PushBack(&first);
    items.PushBack(&second);
    items.PushBack(&third);

    wheels::StringBuilder builder;
    for (Item* item : items) {
      builder << item->data << " ";
    }
    std::string message = builder;
    ASSERT_EQ(message, "hello world ! ");

    items.UnlinkAll();
  }

  SIMPLE_TEST(ReverseIterator) {
    IntrusiveList<Item> items;

    Item a{"a"};
    Item b{"b"};
    Item c{"c"};

    items.PushBack(&a);
    items.PushBack(&b);
    items.PushBack(&c);

    {
      auto iter = items.rbegin();

      auto end = items.rend();

      ASSERT_NE(iter, end);
      ASSERT_EQ(iter->data, "c");
      ++iter;
      ASSERT_NE(iter, end);
      ASSERT_EQ(iter->data, "b");
      ++iter;
      ASSERT_NE(iter, end);
      ASSERT_EQ(iter->data, "a");
      ++iter;
      ASSERT_EQ(iter, end);
    }

    items.UnlinkAll();
  }

  SIMPLE_TEST(AppendList) {
    Item first("hello");
    Item second("world");
    Item third("!");

    IntrusiveList<Item> items;

    ASSERT_EQ(items.begin(), items.end());

    items.PushBack(&first);
    items.PushBack(&second);
    items.PushBack(&third);

    Item foo("foo");
    Item bar("bar");

    IntrusiveList<Item> another_items;
    another_items.PushBack(&foo);
    another_items.PushBack(&bar);

    items.Append(another_items);

    ASSERT_TRUE(another_items.IsEmpty());

    ASSERT_EQ(items.PopFront()->data, "hello");
    ASSERT_EQ(items.PopFront()->data, "world");
    ASSERT_EQ(items.PopFront()->data, "!");
    ASSERT_EQ(items.PopFront()->data, "foo");
    ASSERT_EQ(items.PopFront()->data, "bar");

    items.UnlinkAll();
  }

  SIMPLE_TEST(MoveCtor) {
    Item first("hello");
    Item second("world");
    Item third("!");

    IntrusiveList<Item> items;

    ASSERT_EQ(items.begin(), items.end());

    items.PushBack(&first);
    items.PushBack(&second);

    IntrusiveList<Item> another_items = std::move(items);

    ASSERT_TRUE(items.IsEmpty());
    items.PushBack(&third);
    ASSERT_FALSE(items.IsEmpty());

    another_items.UnlinkAll();
    items.UnlinkAll();
  }

  SIMPLE_TEST(NonEmpty) {
    IntrusiveList<Item> list;

    ASSERT_TRUE(list.IsEmpty());
    ASSERT_FALSE(list.NonEmpty());

    Item hello("Hello");

    list.PushBack(&hello);

    ASSERT_FALSE(list.IsEmpty());
    ASSERT_TRUE(list.NonEmpty());

    list.PopFront();
  }

  SIMPLE_TEST(Swap) {
    {
      IntrusiveList<Item> x{};
      IntrusiveList<Item> y{};
      x.Swap(y);

      ASSERT_TRUE(x.IsEmpty());
      ASSERT_TRUE(y.IsEmpty());
    }

    {
      IntrusiveList<Item> x{};
      IntrusiveList<Item> y{};

      Item a{"a"};
      Item b{"b"};
      x.PushBack(&a);
      x.PushBack(&b);

      x.Swap(y);

      ASSERT_TRUE(x.IsEmpty());

      {
        ASSERT_FALSE(y.IsEmpty());
        ASSERT_EQ(y.Size(), 2);

        Item* f = y.PopFront();
        ASSERT_EQ(f->data, "a");
        Item* s = y.PopFront();
        ASSERT_EQ(s->data, "b");

        ASSERT_TRUE(y.IsEmpty());
      }
    }

    {
      IntrusiveList<Item> x{};
      IntrusiveList<Item> y{};

      Item a{"a"};
      Item b{"b"};
      y.PushBack(&a);
      y.PushBack(&b);

      x.Swap(y);

      ASSERT_TRUE(y.IsEmpty());

      {
        ASSERT_FALSE(x.IsEmpty());
        ASSERT_EQ(x.Size(), 2);

        Item* f = x.PopFront();
        ASSERT_EQ(f->data, "a");
        Item* s = x.PopFront();
        ASSERT_EQ(s->data, "b");

        ASSERT_TRUE(x.IsEmpty());
      }
    }

    {
      IntrusiveList<Item> x{};
      IntrusiveList<Item> y{};

      Item a{"a"};
      Item b{"b"};

      x.PushBack(&a);
      y.PushBack(&b);

      x.Swap(y);

      {
        ASSERT_EQ(x.Size(), 1);

        Item* f = x.PopFront();
        ASSERT_EQ(f->data, "b");

        ASSERT_TRUE(x.IsEmpty());
      }

      {
        ASSERT_EQ(y.Size(), 1);

        Item* f = y.PopFront();
        ASSERT_EQ(f->data, "a");

        ASSERT_TRUE(y.IsEmpty());
      }
    }
  }

  SIMPLE_TEST(Sort) {
    auto less = [](Item* x, Item* y) {
      return x->data < y->data;
    };

    {
      IntrusiveList<Item> items{};
      items.Sort(less);

      ASSERT_TRUE(items.IsEmpty());
    }

    {
      IntrusiveList<Item> items{};
      Item a{"a"};
      Item b{"b"};
      Item c{"c"};

      items.PushBack(&c);
      items.PushBack(&b);
      items.PushBack(&a);

      items.Sort(less);

      {
        Item* x = items.PopFront();
        ASSERT_EQ(x->data, "a");
        Item* y = items.PopFront();
        ASSERT_EQ(y->data, "b");
        Item* z = items.PopFront();
        ASSERT_EQ(z->data, "c");
      }
    }
  }

  struct XTag {};
  struct YTag {};
  struct ZTag {};

  struct TaggedItem
      : IntrusiveListNode<TaggedItem, XTag>,
        IntrusiveListNode<TaggedItem, YTag> {
    int data;

    TaggedItem(int d) : data(d) {
    }
  };

  SIMPLE_TEST(Tags) {
    IntrusiveList<TaggedItem, XTag> x;
    IntrusiveList<TaggedItem, YTag> y;

    IntrusiveList<TaggedItem, ZTag> z;

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

  SIMPLE_TEST(UnlinkTagged) {
    using XList = IntrusiveList<TaggedItem, XTag>;
    using YList = IntrusiveList<TaggedItem, YTag>;

    XList x;
    YList y;

    TaggedItem item{3};

    x.PushBack(&item);
    y.PushBack(&item);

    // Static
    XList::Unlink(&item);

    ASSERT_TRUE(y.NonEmpty());

    y.Unlink(&item);
  }
}
