#include <wheels/test/framework.hpp>

#include <twist/cross.hpp>

#include <twist/ed/c/malloc.hpp>

TEST_SUITE(Malloc) {
  void Write(void* ptr, size_t size) {
    char* buf = (char*)ptr;
    buf[0] = 0x1;
    buf[size - 1] = 0x1;
  }

  SIMPLE_TEST(JustWorks) {
    twist::cross::Run([] {
      void* p = twist::ed::c::malloc(17);
      Write(p, 17);
      twist::ed::c::free(p);
    });
  }

  SIMPLE_TEST(SmallSizes) {
    twist::cross::Run([] {
      for (size_t s = 1; s <= 10; ++s) {
        void* p = twist::ed::c::malloc(s);
        Write(p, s);
        twist::ed::c::free(p);
      }
    });
  }

  SIMPLE_TEST(PowerOf2Sizes) {
    twist::cross::Run([] {
      for (size_t i = 0; i < 12; ++i) {
        void* p = twist::ed::c::malloc(1 << i);
        Write(p, 1 << i);
        twist::ed::c::free(p);
      }
    });
  }
}
