#pragma once

#include "chains/module.hpp"

namespace chains {

namespace wire {

struct Module
{
  template <class T, class Inputs>
  struct Processor
  {
    Processor(const Inputs&, double) {}

    auto tick(T in) const { return in; }
  };
};

} // wire

using Wire = wire::Module;

} // chains
