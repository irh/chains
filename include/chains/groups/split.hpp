#pragma once

#include "chains/module_group.hpp"
#include "chains/processor_group.hpp"

#include <boost/hana/unpack.hpp>

#include <array>

namespace chains {

template <class T, class Processors>
struct SplitProcessor : ProcessorGroup<Processors>
{
  using ProcessorGroup<Processors>::ProcessorGroup;

  auto tick(const T& in = T(0))
  {
    return boost::hana::unpack(
      this->processors_,
      [&in](auto... processors) {
        return std::array<T, sizeof...(processors)>{{processors.tick(in)...}};
      });
  }
};

template <class... Modules>
auto split(Modules... modules)
{
  return ModuleGroup<SplitProcessor, Modules...>(modules...);
}

} // chains
