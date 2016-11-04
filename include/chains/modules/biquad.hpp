#pragma once

#include "chains/dsp/biquad.hpp"
#include "chains/module.hpp"

#include <cassert>

namespace chains {
namespace biquad {

struct Frequency : CallbackParameter
{
  static auto name() { return "Frequency"; }
  static auto minimumValue() { return 20.0; }
  static auto maximumValue() { return 20e3; }
  static auto defaultValue() { return 20e3; }
};

struct Q : CallbackParameter
{
  static auto name() { return "Q"; }
  static auto defaultValue() { return 20e3; }
  static auto minimumValue() { return 20.0; }
};

struct Type : CallbackParameter
{
  static auto name() { return "Type"; }
  static auto defaultValue() { return 0.0; }
  static auto minimumValue() { return 0.0; }
  static auto maximumValue() { return 3.0; }
};

struct Module
{
  using Parameters = ParameterTraits<Frequency, Q, Type>;

  template <class T, class Inputs>
  struct Processor
  {
    Processor(const Inputs& inputs, double sampleRate)
      : inputs_(inputs), biquad_(sampleRate)
    {
    }

    void init()
    {
      auto update = [this](double) { updateFilter(); };
      setCallback<Frequency>(inputs_, update);
      setCallback<Q>(inputs_, update);
      setCallback<Type>(inputs_, update);
    }

    auto tick(const T& in) { return biquad_.tick(in); }

    void updateFilter()
    {
      biquad_.setFilter(filterType(), getValue<Frequency>(inputs_), getValue<Q>(inputs_));
    }

    auto filterType() const
    {
      switch (int(getValue<Type>(inputs_))) {
      case 0: return dsp::Biquad<T>::Type::LowPass;
      case 1: return dsp::Biquad<T>::Type::BandPass;
      case 2: return dsp::Biquad<T>::Type::HighPass;
      case 3: return dsp::Biquad<T>::Type::AllPass;
      default: assert(false); return dsp::Biquad<T>::Type::LowPass;
      }
    }

    Inputs inputs_;
    dsp::Biquad<T> biquad_;
  };
}; // Module

} // biquad

using Biquad = biquad::Module;

} // chains
