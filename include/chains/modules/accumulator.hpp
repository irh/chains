#pragma once

#include "chains/module.hpp"

namespace chains {

namespace accumulator {

struct Amount
{
  static auto name() { return "Amount"; }
  static auto defaultValue() { return 1.0; }
};

struct Wrap
{
  static auto name() { return "Wrap"; }
  static auto defaultValue() { return 1.0; }
};

struct Module
{
  using Parameters = ParameterTraits<Amount, Wrap>;

  template <class T, class Inputs>
  struct Processor
  {
    Processor(const Inputs& inputs, double /* sampleRate */) : inputs_(inputs) {}

    auto tick(const T& in)
    {
      const auto amount = getValue<Amount>(inputs_);
      const auto wrap = getValue<Wrap>(inputs_);

      current_ += in * amount;
      if (current_ >= wrap) {
        current_ -= wrap;
      }

      return current_;
    }

    Inputs inputs_;
    T current_ = T(0);
  };
};

} // accumulator

using Accumulator = accumulator::Module;

} // chains
