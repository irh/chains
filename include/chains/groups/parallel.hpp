#pragma once

#include "chains/module_group.hpp"
#include "chains/processor_group.hpp"

#include <boost/hana/fold.hpp>

namespace chains {

template <class T, class Processors>
struct ParallelProcessor : ProcessorGroup<Processors>
{
  using ProcessorGroup<Processors>::ProcessorGroup;

  auto tick(const T& in = T(0))
  {
    return boost::hana::fold(
      this->processors_, T(0),
      [&in](const T& result, auto& processor) { return result + processor.tick(in); });
  }
};

template <class... Modules>
auto parallel(Modules... modules)
{
  return ModuleGroup<ParallelProcessor, Modules...>(modules...);
}

} // chains
