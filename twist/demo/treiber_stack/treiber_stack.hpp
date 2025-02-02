#pragma once

// Synchronization
#include <twist/ed/std/atomic.hpp>

// Checks
#include <twist/assist/shared.hpp>
#include <twist/assist/memory.hpp>

// Tracing
#include <twist/trace/scope.hpp>
#include <twist/trace/attr/fmt.hpp>
#include <twist/trace/attr/ptr.hpp>

// Treiber lock-free stack

template <typename T>
class LockFreeStack {
  struct Node {
    twist::assist::Shared<T> datum;
    Node* next;
  };

 public:
  LockFreeStack() {
    // stack_.SetVisibility(-1);
  }

  void Push(T datum) {
    twist::trace::Scope push{stack_};
    twist::trace::attr::Fmt datum_attr{"datum", "{}", datum};
    push.LinkAttr(datum_attr);

    Node* new_top = new Node{std::move(datum), top_.load(std::memory_order::relaxed)};

    twist::trace::attr::Ptr new_node_attr{"node", new_top};
    push.LinkAttr(new_node_attr);

    push.Here();

    while (!top_.compare_exchange_weak(new_top->next, new_top, std::memory_order::release)) {
      // Try again
      push.Here();
    }
  }

  std::optional<T> TryPop() {
    twist::trace::Scope try_pop{stack_};

    while (true) {
      Node* top = top_.load(std::memory_order::acquire);

      if (top == nullptr) {
        return std::nullopt;
      }

      Node* top_next = twist::assist::Ptr(top)->next;  // <- Memory check (heap-use-after-free)

      if (top_.compare_exchange_weak(top, top_next, std::memory_order::relaxed)) {
        try_pop.Note("TryPop -> {}", fmt::ptr(top));
        twist::assist::Access(top);  // <- Memory check (heap-use-after-free)
        T datum = std::move(*(top->datum));
        delete top;  // <- Automatic memory check (double-free)
        // Retire(top);
        return datum;
      }
    }
  }

  ~LockFreeStack() {
    twist::trace::Scope dtor{stack_};

    while (TryPop()) {}
    Free();
  }

 private:
  void Retire(Node* node) {
    node->next = retire_top_.load(std::memory_order::relaxed);
    while (!retire_top_.compare_exchange_weak(node->next, node, std::memory_order::release)) {
      // Try again
    }
  }

  void Free() {
    Node* top = retire_top_.load(std::memory_order::relaxed);
    while (top != nullptr) {
      Node* next = top->next;
      delete top;
      top = next;
    }
  }

 private:
  twist::ed::std::atomic<Node*> top_{nullptr};
  twist::ed::std::atomic<Node*> retire_top_{nullptr};
  twist::trace::Domain stack_{"LockFreeStack"};
};
