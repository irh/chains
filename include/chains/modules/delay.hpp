#pragma once

#include "chains/module.hpp"

#include "boost/circular_buffer.hpp"

namespace chains {

namespace delay {

static const int bufferSize = 4096;

struct Length
{
  static auto name() { return "Length"; }
  static auto defaultValue() { return 0.0; }
  static auto minimumValue() { return 0.0; }
  static auto maximumValue() { return double(bufferSize); }
};

struct Module
{
  using Parameters = ParameterTraits<Length>;

  template <class T, class Inputs>
  struct Processor
  {
    Processor(const Inputs& inputs, double sampleRate)
      : inputs_(inputs), buffer_(bufferSize)
    {
      buffer_.resize(bufferSize, T(0));
    }

    auto tick(T in)
    {
      buffer_.push_back(in);
      return *(buffer_.rbegin() + std::size_t(getValue<Length>(inputs_)));
    }

    Inputs inputs_;
    boost::circular_buffer<T> buffer_;
  };
};

} // delay

using Delay = delay::Module;

} // chains
