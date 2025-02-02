#pragma once

#include "domain.hpp"
#include "loc.hpp"
#include "context.hpp"
#include "attr.hpp"
#include "id.hpp"

#include "attr/uint.hpp"

#include "note_context.hpp"

#include "detail/tmp_format.hpp"

#include <type_traits>

namespace twist::trace {

class Scope {
 public:
  // Enter domain
  Scope(const Domain& domain, const char* name, Loc loc = Loc::Current());
  Scope(const Domain& domain, Loc loc = Loc::Current())
      : Scope(domain, loc.Function(), loc) {
  }

  // Inherit domain from the parent scope
  explicit Scope(const char* name, Loc loc = Loc::Current());
  explicit Scope(Loc loc = Loc::Current())
      : Scope(loc.Function(), loc) {
  }

  // Non-copyable
  Scope(const Scope&) = delete;
  Scope& operator=(const Scope&) = delete;

  // Non-movable
  Scope(Scope&&) = delete;
  Scope& operator=(Scope&&) = delete;

  ~Scope();

  static Scope* TryCurrent() noexcept;

  Id GetId() const noexcept {
    return id_;
  }

  const char* GetName() const noexcept {
    return name_;
  }

  Loc GetLoc() const noexcept {
    return loc_;
  }

  Domain* GetDomain() const noexcept {
    return domain_;
  }

  Scope* GetParent() const noexcept {
    return parent_;
  }

  Context GetContext() const noexcept {
    return context_;
  }

  bool IsRoot() const noexcept {
    return parent_ == nullptr;
  }

  Scope& LinkAttr(AttrBase& attr) noexcept {
    attr.next = attr_head_;
    attr_head_ = &attr;
    return *this;
  }

  AttrBase* GetAttrs() const noexcept {
    return attr_head_;
  }

  int GetVisibility() const noexcept {
    return context_.visibility;
  }

  void SetVisibility(int val) noexcept {
    context_.visibility = val;
  }

  int GetEventVisibility() const noexcept;

  bool IsEventVisible() const noexcept {
    return GetEventVisibility() > 0;
  }

  void Here(Loc loc = Loc::Current()) {
    if (IsEventVisible()) {
      Trace("Here", loc);
    }
  }

  // ~ Here()
  void operator()(Loc loc = Loc::Current()) {
    Here(loc);
  }

  void Note(std::string_view text, NoteCtx ctx = {Loc::Current()}) {
    if (IsEventVisible()) {
      Trace(text, ctx.loc);
    }
  }

  template <typename A, typename std::enable_if_t<!std::is_same_v<A, NoteCtx>, bool> = true>
  void Note(::fmt::format_string<A> format_str, A&& a, NoteCtx ctx = {Loc::Current()}) {
    NoteImpl(ctx, format_str, std::forward<A>(a));
  }

  template <typename A, typename B, typename std::enable_if_t<!std::is_same_v<B, NoteCtx>, bool> = true>
  void Note(::fmt::format_string<A, B> format_str, A&& a, B&& b, NoteCtx ctx = {Loc::Current()}) {
    NoteImpl(ctx, format_str, std::forward<A>(a), std::forward<B>(b));
  }

  template <typename A, typename B, typename C, typename std::enable_if_t<!std::is_same_v<C, NoteCtx>, bool> = true>
  void Note(::fmt::format_string<A, B, C> format_str, A&& a, B&& b, C&& c, NoteCtx ctx = {Loc::Current()}) {
    NoteImpl(ctx, format_str, std::forward<A>(a), std::forward<B>(b), std::forward<C>(c));
  }

  template <typename A, typename B, typename C, typename D, typename std::enable_if_t<!std::is_same_v<D, NoteCtx>, bool> = true>
  void Note(::fmt::format_string<A, B, C, D> format_str, A&& a, B&& b, C&& c, D&& d, NoteCtx ctx = {Loc::Current()}) {
    NoteImpl(ctx, format_str, std::forward<A>(a), std::forward<B>(b), std::forward<C>(c), std::forward<D>(d));
  }

  template <typename A, typename B, typename C, typename D, typename E>
  void Note(::fmt::format_string<A, B, C, D, E> format_str, A&& a, B&& b, C&& c, D&& d, E&& e, NoteCtx ctx = {Loc::Current()}) {
    NoteImpl(ctx, format_str, std::forward<A>(a), std::forward<B>(b), std::forward<C>(c), std::forward<D>(d), std::forward<E>(e));
  }

  template <typename ... Args>
  void NoteInline(::fmt::format_string<Args...> format_str,
                Args&&... args) {
    NoteImpl({loc_}, format_str, std::forward<Args>(args)...);
  }

 private:
  template <typename ... Args>
  void NoteImpl(NoteCtx ctx, ::fmt::format_string<Args...> format_str,
                    Args&&... args) {
    if (IsEventVisible()) {
      Trace(detail::TmpFormat(format_str, std::forward<Args>(args)...), ctx.loc);
    }
  }

  void Trace(std::string_view message, Loc loc);

 private:
  const char* name_;
  const Loc loc_;
  Domain* domain_;
  Scope* parent_;
  Id id_;
  Context context_;
  AttrBase* attr_head_ = nullptr;
};

}  // namespace twist::trace
