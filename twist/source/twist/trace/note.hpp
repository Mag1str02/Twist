#pragma once

#include "runtime.hpp"

#include "note_context.hpp"

#include "detail/tmp_format.hpp"

#include <type_traits>

namespace twist::trace {

namespace detail {

template <typename... Args>
void NoteImpl(NoteCtx ctx, ::fmt::format_string<Args...> format_str,
                  Args&&... args) {
  if (Scope* curr = Scope::TryCurrent(); curr != nullptr) {
    if (curr->IsEventVisible()) {
      Event event{curr, ctx.loc,
                  detail::TmpFormat(format_str, std::forward<Args>(args)...)};
      rt::Trace(&event);
    }
  }
}

}  // namespace detail

inline void Note(::fmt::format_string<> format_str, NoteCtx ctx = {Loc::Current()}) {
  detail::NoteImpl(ctx, format_str);
}

template <typename A, typename std::enable_if_t<!std::is_same_v<A, NoteCtx>, bool> = true>
void Note(::fmt::format_string<A> format_str, A&& a, NoteCtx ctx = {Loc::Current()}) {
  detail::NoteImpl(ctx, format_str, std::forward<A>(a));
}

template <typename A, typename B, typename std::enable_if_t<!std::is_same_v<B, NoteCtx>, bool> = true>
void Note(::fmt::format_string<A, B> format_str, A&& a, B&& b, NoteCtx ctx = {Loc::Current()}) {
  detail::NoteImpl(ctx, format_str, std::forward<A>(a), std::forward<B>(b));
}

template <typename A, typename B, typename C, typename std::enable_if_t<!std::is_same_v<C, NoteCtx>, bool> = true>
void Note(::fmt::format_string<A, B, C> format_str, A&& a, B&& b, C&& c, NoteCtx ctx = {Loc::Current()}) {
  detail::NoteImpl(ctx, format_str, std::forward<A>(a), std::forward<B>(b), std::forward<C>(c));
}

template <typename A, typename B, typename C, typename D, typename std::enable_if_t<!std::is_same_v<D, NoteCtx>, bool> = true>
void Note(::fmt::format_string<A, B, C> format_str, A&& a, B&& b, C&& c, D&& d, NoteCtx ctx = {Loc::Current()}) {
  detail::NoteImpl(ctx, format_str, std::forward<A>(a), std::forward<B>(b), std::forward<C>(c), std::forward<D>(d));
}

template <typename A, typename B, typename C, typename D, typename E, typename std::enable_if_t<!std::is_same_v<E, NoteCtx>, bool> = true>
void Note(::fmt::format_string<A, B, C> format_str, A&& a, B&& b, C&& c, D&& d, E&& e, NoteCtx ctx = {Loc::Current()}) {
  detail::NoteImpl(ctx, format_str, std::forward<A>(a), std::forward<B>(b), std::forward<C>(c), std::forward<D>(d), std::forward<E>(e));
}

// Deprecated

template <typename ... Args>
void JustNote(fmt::format_string<Args...> /*format_str*/, Args&&... /*args*/) {
  static_assert(false, "deprecated, use twist::trace::Note instead");
}

}  // namespace twist::trace
