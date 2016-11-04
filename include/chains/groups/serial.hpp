#pragma once

#include "chains/module_group.hpp"
#include "chains/processor_group.hpp"

namespace chains {

template <class T, class Processors>
struct SerialProcessor : ProcessorGroup<Processors>
{
  using ProcessorGroup<Processors>::ProcessorGroup;

  auto tick(const T& in = T(0))
  {
    return boost::hana::unpack(this->processors_, [&in](auto&... processors) {
      return tickHelper(in, processors...);
    });
  }

private:
  template <class TIn, class TProcessor, class... TProcessors>
  static auto tickHelper(const TIn& in, TProcessor& processor, TProcessors&... rest)
  {
    return tickHelper(processor.tick(in), rest...);
  }

  template <class TIn, class... TProcessors>
  static auto tickHelper(const TIn& in, TProcessors&...)
  {
    return in;
  }
};

template <class... Modules>
auto serial(Modules... modules)
{
  return ModuleGroup<SerialProcessor, Modules...>(modules...);
}

} // chains
