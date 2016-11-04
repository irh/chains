#pragma once

#include "chains/parameter.hpp"
#include "chains/support/estd.hpp"
#include "chains/support/type_is_in_pack.hpp"

namespace chains {

// Expose is used when specifying signal chains to expose a module's parameters,
template <class... ParameterTraits>
struct Expose
{
};

namespace detail {

template <class Parameters, class... Exposed>
struct ExposedParameterCheck;

// Specializing Parameters here to expose the underlying types
template <class... Parameters, class... Exposed>
struct ExposedParameterCheck<ParameterTraits<Parameters...>, Exposed...>
{
  static constexpr bool value =
    estd::conjunction_v<TypeIsInPack<Exposed, typename Parameters::Traits...>...>;
};

} // detail

// Evaluates to true if all Exposed parameters are in the parameters list
template <class Parameters, class... Exposed>
constexpr bool exposedParameterCheck =
  detail::ExposedParameterCheck<Parameters, Exposed...>::value;

} // chains
