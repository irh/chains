#pragma once

#include "chains/dsp/phasor.hpp"
#include "chains/module.hpp"

namespace chains {

namespace phasor {

struct Frequency : CallbackParameter
{
  static auto name() { return "Frequency"; }
  static auto defaultValue() { return 1.0; }
  static auto minimumValue() { return 0.0; }
};

struct Module
{
  using Parameters = ParameterTraits<Frequency>;

  template <class T, class Inputs>
  struct Processor
  {
    Processor(const Inputs& inputs, double sampleRate)
      : inputs_(inputs), phasor_(sampleRate)
    {
    }

    void init()
    {
      setCallback<Frequency>(
        inputs_, [this](double value) { phasor_.setFrequency(value); });
    }

    auto tick(T /*in*/) { return phasor_.tick(); }

    Inputs inputs_;
    dsp::Phasor<T> phasor_;
  };
};

} // phasor

using Phasor = phasor::Module;

} // chains
