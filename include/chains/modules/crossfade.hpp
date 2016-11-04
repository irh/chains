#pragma once

#include "chains/module.hpp"

namespace chains {

namespace crossfade {

struct Fade
{
  static auto name() { return "Fade"; }
  static auto defaultValue() { return 0.0; }
};

struct Module
{
  using Parameters = ParameterTraits<Fade>;

  template <class T, class Inputs>
  struct Processor
  {
    Processor(const Inputs& inputs, double /* sampleRate */) : inputs_(inputs) {}

    auto tick(const std::array<T, 2>& in)
    {
      const auto fade = getValue<Fade>(inputs_);
      const auto fadeInv = T(1) - fade;

      // TODO non-linear fades
      return in[0] * fadeInv + in[1] * fade;
    }

    Inputs inputs_;
  };
};

} // crossfade

using Crossfade = crossfade::Module;

} // chains
