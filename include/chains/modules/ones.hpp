#pragma once

#include "chains/module.hpp"

namespace chains {

namespace ones {

struct Module
{
  template <class T, class Inputs>
  struct Processor
  {
    Processor(const Inputs&, double /* sampleRate */) {}

    auto tick(T) const { return T(1); }
  };
};

} // ones

using Ones = ones::Module;

} // chains
