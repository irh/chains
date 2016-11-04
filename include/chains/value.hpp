#pragma once

namespace chains {

// Value is used to declare an initial value for a module's parameter
template <class ParameterTraits>
struct Value
{
  explicit Value(const double value) : value_(value) {}

  double value_;
};

namespace detail {

template <class Parameters, class... Values>
struct ValueParameterCheck;

// Specializing Parameters here to expose the underlying types
template <class... Parameters, class... Values>
struct ValueParameterCheck<ParameterTraits<Parameters...>, Values...>
{
  static constexpr bool value =
    estd::conjunction_v<TypeIsInPack<Values, Value<Parameters>...>...>;
};

} // detail

// Evaluates to true if all Values represent parameters in the parameters list
template <class Parameters, class... Values>
constexpr bool valueParameterCheck =
  detail::ValueParameterCheck<Parameters, Values...>::value;

} // chains
