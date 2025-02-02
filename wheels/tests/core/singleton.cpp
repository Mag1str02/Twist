#include <wheels/test/framework.hpp>

#include <wheels/core/singleton.hpp>

TEST_SUITE(Singleton) {
  struct Box {
    void Set(std::string data) {
      data_ = data;
    }

    std::string Get() const {
      return data_;
    }

   private:
    std::string data_;
  };

  SIMPLE_TEST(Leaky) {
    LeakySingleton<Box>().Set("Just works");
    ASSERT_EQ(LeakySingleton<Box>().Get(), "Just works");
  }
}
