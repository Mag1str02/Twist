#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <vector>
#include <deque>
#include <list>
#include <set>

TEST_SUITE(New) {
  SIMPLE_TEST(NewInt) {
    twist::cross::Run([] {
      {
        int* ptr = new int{4};
        ASSERT_TRUE(ptr != nullptr);
        ASSERT_EQ(*ptr, 4);
        delete ptr;
      }
    });
  }

  SIMPLE_TEST(NewArray) {
    twist::cross::Run([] {
      {
        int* arr = new int[128];
        ASSERT_TRUE(arr != nullptr);

        for (size_t i = 0; i < 127; ++i) {
          arr[i] = i;
        }

        delete[] arr;
      }
    });
  }

  struct Empty {};

  SIMPLE_TEST(NewEmpty) {
    twist::cross::Run([] {
      Empty* e1 = new Empty{};
      Empty* e2 = new Empty{};

      ASSERT_TRUE(e1 != nullptr);
      ASSERT_TRUE(e2 != nullptr);

      ASSERT_TRUE(e1 != e2);

      delete e1;
      delete e2;
    });
  }

  SIMPLE_TEST(DeleteNullptr) {
    twist::cross::Run([] {
      int* null_ptr = nullptr;
      delete null_ptr;
    });
  }

  SIMPLE_TEST(StdContainers) {
    twist::cross::Run([] {
      {
        // std::vector
        std::vector<int> vec;
        for (int i = 0; i < 256; ++i) {
          vec.push_back(i);
        }
      }

      {
        // std::deque
        std::deque<int> deq;
        for (int i = 0; i < 256; ++i) {
          deq.push_back(i);
        }
      }

      {
        // std::list
        std::list<int> list;
        for (int i = 0; i < 256; ++i) {
          list.push_back(i);
        }
      }

      {
        // std::set
        std::set<int> set;
        for (int i = 0; i < 256; ++i) {
          set.insert(i);
        }
      }
    });
  }

  template <size_t Size>
  struct Box {
    unsigned char buf[Size];

    void Write(size_t index) {
      buf[index] = 0xFF;
    }
  };

  template <size_t Size>
  void NewBox() {
    static_assert(sizeof(Box<Size>) == Size);

    Box<Size>* box = new Box<Size>{};
    ASSERT_TRUE(box != nullptr);

    box->Write(0);
    box->Write(Size - 1);

    delete box;
  }

  SIMPLE_TEST(SmallSizes) {
    twist::cross::Run([] {
      NewBox<1>();
      NewBox<2>();
      NewBox<3>();
      NewBox<4>();
      NewBox<5>();
      NewBox<6>();
      NewBox<7>();
    });
  }

  SIMPLE_TEST(PowerOf2Sizes) {
    twist::cross::Run([] {
      // https://oeis.org/A000079
      NewBox<8>();
      NewBox<16>();
      NewBox<32>();
      NewBox<64>();
      NewBox<128>();
      NewBox<256>();
      NewBox<512>();
      NewBox<1024>();
      NewBox<2048>();
      NewBox<4096>();
      NewBox<8192>();
      NewBox<16384>();
      NewBox<32768>();
      NewBox<65536>();
      NewBox<131072>();
      NewBox<262144>();
      NewBox<524288>();
      NewBox<1048576>();
    });
  }
}
