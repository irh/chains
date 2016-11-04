#pragma once

#include "chains/module.hpp"

#include <iostream>

namespace chains {

namespace probe {

struct Module
{
  template <class T, class Inputs>
  struct Processor
  {
    Processor(const Inputs&, double) {}

    auto tick(const T in) const
    {
      std::cout << "Probe: " << this << " - " << in << '\n';
      return in;
    }
  };
};

} // probe

using Probe = probe::Module;

} // chains
