#pragma once

#include <wheels/core/assert.hpp>

#include <cstddef>
#include <stdexcept>
#include <iterator>

namespace wheels {

struct IntrusiveListDefaultTag {};

// Introduction to intrusive containers:
// https://www.boost.org/doc/libs/1_67_0/doc/html/intrusive/intrusive_vs_nontrusive.html

// Represents node of circular doubly-linked list

template <typename T, typename Tag = IntrusiveListDefaultTag>
struct IntrusiveListNode {
  using Node = IntrusiveListNode;

  Node* prev_ = nullptr;
  Node* next_ = nullptr;

  Node* Prev() noexcept {
    return prev_;
  }

  Node* Next() noexcept {
    return next_;
  }

  // Links this node before next in list

  void LinkBefore(Node* next) noexcept {
    WHEELS_VERIFY(!IsLinked(), "cannot link already linked node");

    prev_ = next->prev_;
    prev_->next_ = this;
    next_ = next;
    next->prev_ = this;
  }

  // Is this node linked in a circular list?

  bool IsLinked() const noexcept {
    return next_ != nullptr;
  }

  // Unlink this node from current list

  void Unlink() noexcept {
    if (next_) {
      next_->prev_ = prev_;
    }
    if (prev_) {
      prev_->next_ = next_;
    }
    next_ = prev_ = nullptr;
  }

  T* AsItem() noexcept {
    return static_cast<T*>(this);
  }
};

// Implemented as circular doubly-linked list with sentinel node

template <typename T, typename Tag = IntrusiveListDefaultTag>
class IntrusiveList {
  using Node = IntrusiveListNode<T, Tag>;

  using List = IntrusiveList<T, Tag>;

 public:
  void PushBack(Node* node) noexcept {
    node->LinkBefore(&head_);
  }

  void PushFront(Node* node) noexcept {
    node->LinkBefore(head_.next_);
  }

  // Returns nullptr if empty
  T* PopFront() noexcept {
    if (IsEmpty()) {
      return nullptr;
    }
    Node* front = head_.next_;
    front->Unlink();
    return front->AsItem();
  }

  // Returns nullptr if empty
  T* PopBack() noexcept {
    if (IsEmpty()) {
      return nullptr;
    }
    Node* back = head_.prev_;
    back->Unlink();
    return back->AsItem();
  }

  // Append (= move, re-link) all nodes from `that` list to the end of this list
  // Post-condition: that.IsEmpty() == true
  void Append(List& that) noexcept {
    if (that.IsEmpty()) {
      return;
    }

    Node* that_front = that.head_.next_;
    Node* that_back = that.head_.prev_;

    that_back->next_ = &head_;
    that_front->prev_ = head_.prev_;

    Node* back = head_.prev_;

    head_.prev_ = that_back;
    back->next_ = that_front;

    that.head_.next_ = that.head_.prev_ = &that.head_;
  }

  bool IsEmpty() const noexcept {
    return head_.next_ == &head_;
  }

  bool NonEmpty() const noexcept {
    return !IsEmpty();
  }

  bool HasItems() const noexcept {
    return !IsEmpty();
  }

  IntrusiveList() {
    InitEmpty();
  }

  IntrusiveList(List&& that) {
    InitEmpty();
    Append(that);
  }

  // Intentionally disabled
  // Be explicit: use UnlinkAll + Append
  IntrusiveList& operator=(List&& that) = delete;

  // Non-copyable
  IntrusiveList(const List& that) = delete;
  IntrusiveList& operator=(const List& that) = delete;

  ~IntrusiveList() {
    WHEELS_ASSERT(IsEmpty(), "List is not empty");
  }

  // Complexity: O(size)
  size_t Size() const {
    return std::distance(begin(), end());
  }

  // Complexity: O(1)
  void Swap(List& with) {
    List tmp;
    tmp.Append(*this);
    Append(with);
    with.Append(tmp);
  }

  // Complexity: O(size)
  template <typename Less>
  void Sort(Less less) {
    List sorted{};

    while (NonEmpty()) {
      Node* candidate = head_.Next();
      WHEELS_ASSERT(candidate != &head_, "List is empty");

      Node* curr = candidate->Next();
      while (curr != &head_) {
        if (less(curr->AsItem(), candidate->AsItem())) {
          candidate = curr;
        }
        curr = curr->Next();
      }

      WHEELS_ASSERT(candidate->IsLinked(), "Candidate unlinked");
      candidate->Unlink();

      sorted.PushBack(candidate);
    }

    Swap(sorted);
  }

  void UnlinkAll() {
    Node* current = head_.next_;
    while (current != &head_) {
      Node* next = current->next_;
      current->Unlink();
      current = next;
    }
  }

  [[deprecated]] void Clear() {
    UnlinkAll();
  }

  // Unlinked tagged node
  static void Unlink(Node* node) {
    node->Unlink();
  }

  static bool IsLinked(Node* node) {
    return node->IsLinked();
  }

  // Iteration

  // See "The Standard Librarian : Defining Iterators and Const Iterators"

  template <bool Next, class NodeT, class ItemT>
  class IteratorImpl {
    using Iterator = IteratorImpl<Next, NodeT, ItemT>;

   public:
    using value_type = ItemT;                             // NOLINT
    using pointer = value_type*;                          // NOLINT
    using reference = value_type*;                        // NOLINT
    using difference_type = ptrdiff_t;                    // NOLINT
    using iterator_category = std::forward_iterator_tag;  // NOLINT

   public:
    IteratorImpl(NodeT* start) : current_(start) {
    }

    // prefix increment
    Iterator& operator++() {
      if constexpr (Next) {
        current_ = current_->next_;
      } else {
        current_ = current_->prev_;
      }
      return *this;
    }

    Iterator& operator--() {
      current_ = current_->prev_;
      return *this;
    }

    bool operator==(const Iterator& that) const {
      return current_ == that.current_;
    }

    bool operator!=(const Iterator& that) const {
      return !(*this == that);
    }

    ItemT* operator*() const {
      return Item();
    }

    ItemT* operator->() const {
      return Item();
    }

    ItemT* Item() const {
      return static_cast<ItemT*>(current_);
    }

   private:
    NodeT* current_;
  };

  // Forward iteration

  using Iterator = IteratorImpl<true, Node, T>;
  using ConstIterator = IteratorImpl<true, const Node, const T>;

  Iterator begin() {  // NOLINT
    return Iterator(head_.next_);
  }

  Iterator end() {  // NOLINT
    return Iterator(&head_);
  }

  ConstIterator begin() const {  // NOLINT
    return ConstIterator(head_.next_);
  }

  ConstIterator end() const {  // NOLINT
    return ConstIterator(&head_);
  }

  // Reverse iterator

  using ReverseIterator = IteratorImpl<false, Node, T>;
  using ConstReverseIterator = IteratorImpl<false, const Node, const T>;

  ReverseIterator rbegin() {  // NOLINT
    return ReverseIterator(head_.prev_);
  }

  ReverseIterator rend() {  // NOLINT
    return ReverseIterator(&head_);
  }

  ConstReverseIterator rbegin() const {  // NOLINT
    return ConstReverseIterator(head_.prev_);
  }

  ConstReverseIterator rend() const {  // NOLINT
    return ConstReverseIterator(&head_);
  }

 private:
  void InitEmpty() {
    head_.next_ = head_.prev_ = &head_;
  }

 private:
  Node head_;  // sentinel node
};

}  // namespace wheels
