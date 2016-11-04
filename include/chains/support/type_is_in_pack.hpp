#pragma once

#include <type_traits>

namespace detail {

template <class T, class... Ts>
struct TypeIsInPack : std::false_type
{
};

template <class T, class... Ts>
struct TypeIsInPack<T, T, Ts...> : std::true_type
{
};

template <class T, class U, class... Ts>
struct TypeIsInPack<T, U, Ts...> : TypeIsInPack<T, Ts...>
{
};

} // detail

template <class T, class... Ts>
using TypeIsInPack =
  std::is_same<typename detail::TypeIsInPack<T, Ts...>::type, std::true_type>;

template <class T, class... Ts>
constexpr bool typeIsInPack = TypeIsInPack<T, Ts...>::value;
