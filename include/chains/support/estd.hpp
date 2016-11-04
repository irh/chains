#pragma once

#include <type_traits>

namespace estd {

template <class T, class U>
constexpr bool is_same_v = std::is_same<T, U>::value;

template <class T, class U>
constexpr bool is_base_of_v = std::is_base_of<T, U>::value;

template <class...>
struct conjunction : std::true_type
{
};

template <class T>
struct conjunction<T> : T
{
};

template <class T, class... Ts>
struct conjunction<T, Ts...>
  : std::conditional_t<T::value != false, conjunction<Ts...>, T>
{
};

template<class... Ts>
constexpr bool conjunction_v = conjunction<Ts...>::value;

} // estd
