#pragma once

#include "chains/module_group.hpp"
#include "chains/processor_group.hpp"

namespace chains {

template <class T, class Processors>
struct RecursiveProcessor : ProcessorGroup<Processors>
{
  using ProcessorGroup<Processors>::ProcessorGroup;

  auto tick(const T& in = T(0))
  {
    using namespace boost::hana::literals;
    const auto forward = this->processors_[0_c].tick(in + previous_);
    previous_ = this->processors_[1_c].tick(forward);
    return forward;
  }

private:
  T previous_ = T(0);
};

template <class Forward, class Back>
auto recursive(Forward&& forward, Back&& back)
{
  return ModuleGroup<RecursiveProcessor, Forward, Back>(forward, back);
}

} // chains
