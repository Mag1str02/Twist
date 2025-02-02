#include <twist/mod/cross.hpp>

#include <fmt/core.h>

#include <twist/wheels/fmt/source_location.hpp>

#if defined(__TWIST_SIM__)

#include <twist/mod/sim.hpp>

namespace twist::cross {

struct CrossRunPrinter : rt::sim::system::log::ILogger {
  void Add(rt::sim::system::log::Event* event) override {
    if (event->user) {
      fmt::println("Thread #{} -- {}:{} -- {}:{} -- {}",
                   event->sched->thread_id,
                   event->source->loc.File(),
                   event->source->loc.Line(),
                   event->scope->GetDomain()->GetName(),
                   event->scope->GetName(),
                   event->descr);
    }
  }
};

void Run(sim::MainRoutine main) {
  auto params = sim::Simulator::Params{};
  // Consistent with mt::Run
  params.crash_on_abort = true;

  sim::sched::RandomScheduler random{};
  sim::Simulator simulator{&random, params};
  CrossRunPrinter printer;
  simulator.SetLogger(&printer);
  simulator.AllowSysLogging(false);
  auto result = simulator.Run(main);

  if (result.Failure()) {
    fmt::println("Simulation failure: {}", result.std_err);
    std::abort();
  }
}

}  // namespace twist::cross

#else

#include <twist/mod/thr.hpp>

namespace twist::cross {

void Run(thr::TestRoutine test) {
  thr::Run(test);
}

}  // namespace twist::run

#endif
