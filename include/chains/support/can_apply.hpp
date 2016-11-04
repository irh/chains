#pragma once

#include <type_traits>

// adapted from http://stackoverflow.com/a/29521342

namespace detail {

template <class...>
struct Types
{
  using type = Types;
};

template <template <class...> class Check, class types, class Enable = void>
struct CanApply : std::false_type
{
};

template <template <class...> class Check, class... Ts>
struct CanApply<Check, Types<Ts...>, std::void_t<Check<Ts...>>> : std::true_type
{
};

} // detail

template <template <class...> class Check, class... Ts>
using canApply = detail::CanApply<Check, detail::Types<Ts...>>;
