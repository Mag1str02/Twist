#include <wheels/test/framework.hpp>
#include <wheels/test/fail_handler.hpp>
#include <wheels/test/util/cpu_time_budget.hpp>

#include <wheels/system/quick_exit.hpp>

#include <wheels/core/stop_watch.hpp>

#include <thread>

using namespace std::chrono_literals;

class Registrar {
 public:
  Registrar() {
    MakeAndRegisterTest();
  }

  void MakeAndRegisterTest() {
    auto body = []() {
      std::cout << "Manually registered test";
    };
    auto test = wheels::test::MakeTest(body, "MakeTest", "TestFramework", wheels::test::TestOptions{});
    wheels::test::RegisterTest(std::move(test));
  }
};

static Registrar registrar;

TEST_SUITE(TestFramework) {
  void ThrowRuntimeError() {
    throw std::runtime_error("error!");
  }

  SIMPLE_TEST(Asserts) {
    ASSERT_TRUE(2 * 2 == 4);
    ASSERT_FALSE(0 > 1);

    ASSERT_EQ(1 + 2, 3);
    ASSERT_NE(2 * 2, 5);

    ASSERT_GT(2, 1);
    ASSERT_LT(1, 2);

    ASSERT_GE(1 + 2, 3);
    ASSERT_LE(1 + 2, 3);

    ASSERT_THROW(ThrowRuntimeError(), std::runtime_error);
  }

  class CountingFailHandler
 : public wheels::test::ITestFailHandler {
   public:
    void Fail(const wheels::test::ITest& /*test*/, const std::string& /*error*/) override {
      ++fail_count_;
    }

    size_t FailCount() const {
      return fail_count_;
    }

   private:
    size_t fail_count_ = 0;
  };

//  SIMPLE_TEST(FiredAsserts) {
//    auto counting_fail_handler = std::make_shared<CountingFailHandler>();
//
//    wheels::test::FailHandlerSwitcher switcher(counting_fail_handler);
//
//    ASSERT_TRUE(false);
//    ASSERT_EQ(counting_fail_handler->FailCount(), 1);
//
//    ASSERT_EQ(1, 2);
//    ASSERT_EQ(counting_fail_handler->FailCount(), 2);
//  }

  /*
  TEST(TimeLimit, wheels::test::TestOptions().TimeLimit(12s)) {
    std::this_thread::sleep_for(11s);
  }
  */

  // SIMPLE_TEST(Fail) { ASSERT_TRUE(false); }

  /*
  TEST(TL, wheels::test::TestOptions().TimeLimit(1s)) {
    std::this_thread::sleep_for(1100ms);
  }
  */

  TEST(TimeLeft, wheels::test::TestOptions().TimeLimit(2s)) {
    while (wheels::test::TestTimeLeft() > 100ms) {
      // KeepRunning
      std::this_thread::sleep_for(10ms);
    }
  }

  TEST(ForceFork, wheels::test::TestOptions().ForceFork()) {
    wheels::QuickExit(0);
  }

  TEST(CpuTimeBudget, wheels::test::TestOptions().TimeLimit(1s)) {
    wheels::test::CpuTimeBudgetGuard g{100ms};

    wheels::StopWatch stop_watch;
    while (stop_watch.Elapsed() < 256ms) {
      std::this_thread::sleep_for(1ms);
    }

    std::cout << "Cpu time usage: " << g.Usage().count() << "us" << std::endl;
  }
}
