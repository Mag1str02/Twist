#pragma once

#include <type_traits>

namespace wheels {

namespace detail {

template <class, template <typename> class>
struct IsInstance : public std::false_type {};

template <class T, template <typename> class Template>
struct IsInstance<Template<T>, Template> : public std::true_type {};

}  // namespace detail

template<class Class, template<typename> class Template>
concept InstantiationOf = detail::IsInstance<Class, Template>::value;

}  // namespace wheels
