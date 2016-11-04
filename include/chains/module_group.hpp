#pragma once

#include <boost/hana/flatten.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/tuple.hpp>

namespace chains {

template <template <class, class> class ProcessorGroup, class... Modules>
struct ModuleGroup
{
  ModuleGroup(Modules... modules)
    : modules_(modules...)
  {
  }

  ModuleGroup(boost::hana::tuple<Modules...> modules)
    : modules_(std::move(modules))
  {
  }

  auto named(const char* name) const
  {
    return ModuleGroup{boost::hana::transform(
      modules_, [&name](const auto& module) { return module.named(name); })};
  }

  template <class T>
  auto makeProcessor(const double sampleRate) const
  {
    auto moduleProcessors = makeProcessors<T>(sampleRate);
    return ProcessorGroup<T, decltype(moduleProcessors)>{moduleProcessors};
  }

  auto exposedParameters() const
  {
    return boost::hana::flatten(boost::hana::transform(
      modules_, [](const auto& module) { return module.exposedParameters(); }));
  }

protected:
  template <class T>
  auto makeProcessors(const double sampleRate) const
  {
    return boost::hana::transform(modules_, [=](const auto& module) {
      return module.template makeProcessor<T>(sampleRate);
    });
  }

  boost::hana::tuple<Modules...> modules_;
};

} // chains
