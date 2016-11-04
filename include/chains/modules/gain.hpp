#pragma once

#include "chains/module.hpp"

namespace chains {

namespace gain {

struct Gain
{
  static auto name() { return "Gain"; }
  static auto defaultValue() { return 1.0; }
};

struct Module
{
  using Parameters = ParameterTraits<Gain>;

  template <class T, class Inputs>
  struct Processor
  {
    Processor(const Inputs& inputs, double /* sampleRate */) : inputs_(inputs) {}

    auto tick(const T& in) { return in * getValue<Gain>(inputs_); }

    Inputs inputs_;
  };
};

} // gain

using Gain = gain::Module;

} // chains
