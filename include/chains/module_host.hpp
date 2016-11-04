#pragma once

#include "chains/parameter.hpp"
#include "chains/processor_host.hpp"

#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/tuple.hpp>

namespace chains {

template <class Traits, class Parameters, class Exposed>
class ModuleHost;

// By specializing ModuleHost, we can retrieve the individual exposed parameters
template <class Traits, class Parameters, class... Exposed>
class ModuleHost<Traits, Parameters, Expose<Exposed...>>
{
  const boost::hana::tuple<Parameter<Exposed>...> exposed_;
  const Parameters parameters_;

  static_assert(exposedParameterCheck<Parameters, Exposed...>,
                "Exposed parameter not found in module's parameter list");

public:
  ModuleHost(const char* moduleName, Parameters parameters)
    : exposed_(boost::hana::make_tuple(Parameter<Exposed>{moduleName}...))
    , parameters_(parameters)
  {
  }

  auto named(const char* name) const { return ModuleHost{name, parameters_}; }

  auto& exposedParameters() const { return exposed_; }

  template <class T>
  auto makeProcessor(const double sampleRate) const
  {
    const auto inputs = makeInputMap(parameters_);

    using Inputs = std::remove_const_t<decltype(inputs)>;
    using Processor = typename Traits::template Processor<T, Inputs>;

    return ProcessorHost<Processor, Inputs, Parameter<Exposed>...>{inputs, sampleRate};
  }

private:
  template <template <class...> class Tuple, class... Parameter>
  auto makeInputMap(const Tuple<Parameter...>& parameters) const
  {
    using namespace boost::hana;
    return make_map(make_pair(
      type_c<typename Parameter::Traits>,
      std::get<Parameter>(parameters)
        .template makeInput<typeIsInPack<typename Parameter::Traits, Exposed...>>())...);
  }
};

} // chains
