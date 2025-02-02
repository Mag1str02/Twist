#include <twist/sim.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cassert>
#include <random>

using namespace std::chrono_literals;

static_assert(twist::build::IsolatedSim());

TEST_CASE("Memory") {
  SECTION("delete nullptr") {
    auto params = twist::sim::SimulatorParams{};
    params.memset_malloc = 0xFF;

    twist::sim::TestSim(params, [] {
      int* p = nullptr;
      delete p;
    });
  }

  SECTION("Free list resize") {
    struct Node {
      Node* next;
    };

    twist::sim::TestSim({}, [] {
      Node* top = nullptr;
      for (size_t i = 0; i < 4096; ++i) {
        Node* new_node = new Node{top};
        top = new_node;
      }

      while (top != nullptr) {
        Node* next = top->next;
        delete top;
        top = next;
      }
    });
  }

  SECTION("Fill memory") {
    auto params = twist::sim::SimulatorParams{};
    params.memset_malloc = 0xFF;

    twist::sim::TestSim(params, [] {
      int* p = new int;
      assert(*p != 0);
      delete p;
    });
  }

  SECTION("Randomization") {
    struct Node {
      // Empty
    };

    auto params = twist::sim::SimulatorParams{};
    params.randomize_malloc = true;

    twist::sim::TestSim(params, [] {
      static const size_t kNodes = 4;

      std::array<Node*, kNodes> nodes;

      {
        // Prepare free list
        for (size_t i = 0; i < kNodes; ++i) {
          nodes[i] = new Node{};
        }
        for (size_t i = 0; i < kNodes; ++i) {
          delete nodes[i];
        }
      }

      std::random_device d{};

      for (size_t i = 0; i < 64; ++i) {
        // Random permutation
        std::shuffle(nodes.begin(), nodes.end(), d);

        while (true) {
          size_t match = 0;
          for (size_t j = 0; j < kNodes; ++j) {
            Node* n = new Node{};
            if (n == nodes[j]) {
              ++match;
            } else {
              delete n;
              break;
            }
          }

          // Cleanup
          for (size_t j = 0; j < match; ++j) {
            delete nodes[j];
          }

          if (match == kNodes) {
            break;
          }
        }
      }
    });
  }

  SECTION("Double-free") {
    auto params = twist::sim::SimulatorParams{};
    params.crash_on_abort = false;

    auto result = twist::sim::TestSim(params, [] {
      int* p = new int{17};
      delete p;
      delete p;  // <- Aborted

      WHEELS_UNREACHABLE();
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == twist::sim::Status::MemoryDoubleFree);
    fmt::println("Stderr: {}", result.std_err);
  }

  SECTION("Memory leak") {
    auto params = twist::sim::SimulatorParams{};
    params.crash_on_abort = false;

    auto result = twist::sim::TestSim(params, [] {
      new int{1};
      new char[128];
    });

    REQUIRE(!result.Ok());
    REQUIRE(result.status == twist::sim::Status::MemoryLeak);
    fmt::println("Stderr: {}", result.std_err);
  }
}
