#pragma once

#include "chains/expose.hpp"
#include "chains/module_host.hpp"
#include "chains/value.hpp"

#include <boost/hana/transform.hpp>

namespace chains {

namespace detail {

// default case, no more values to compare, leave parameter unmodified
template <class ParameterTraits, class... Values>
auto makeParameter(const ParameterTraits&, const Values&...)
{
  return Parameter<ParameterTraits>{};
}

// matching parameter found for value, return a parameter with the specified value
template <class ParameterTraits, class... Values>
auto makeParameter(const ParameterTraits&,
                   const Value<ParameterTraits>& value,
                   const Values&...)
{
  return Parameter<ParameterTraits>{value.value_};
}

// value for another parameter, move on to the next one
template <class ParameterTraits, class ValueForDifferentParameter, class... Values>
auto makeParameter(const ParameterTraits& parameter,
                   const ValueForDifferentParameter&,
                   const Values&... values)
{
  return makeParameter(parameter, values...);
}

// Default case used when module hasn't specified any parameters
template <class ModuleTraits, class = void>
struct ModuleParameters
{
  using type = ParameterTraits<>;
};

template <class ModuleTraits>
struct ModuleParameters<ModuleTraits, std::void_t<typename ModuleTraits::Parameters>>
{
  using type = typename ModuleTraits::Parameters;
};

} // detail

// module is used to declare a module in a signal chain
template <class ModuleTraits, class Exposed = Expose<>, class... Values>
auto module(const char* moduleName, const Values&... values)
{
  // Get the module's parameters, if specified
  using ModuleParameters = typename detail::ModuleParameters<ModuleTraits>::type;

  static_assert(valueParameterCheck<ModuleParameters, Values...>,
                "Value for parameter not found in module's parameter list");

  // Make a list of parameters from the module's parameter traits, applying any
  // user-specified values
  auto parameters =
    boost::hana::transform(ModuleParameters{}, [&values...](const auto& parameterTraits) {
      return detail::makeParameter(parameterTraits, values...);
    });

  return ModuleHost<ModuleTraits, decltype(parameters), Exposed>{moduleName, parameters};
}

// A module creator for unnamed modules
template <class Traits, class Exposed = Expose<>, class... Values>
auto module(const Values&... values)
{
  return module<Traits, Exposed>("", values...);
}

} // chains
