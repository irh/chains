#pragma once

#include <cassert>

namespace dsp
{

template<class T>
struct Phasor {
  Phasor(double sampleRate)
    : sampleRate_(sampleRate)
  {}

  void setFrequency(const T frequency) {
    inc_ = frequency / sampleRate_;
    assert(inc_ >= T(-1) && inc_ <= T(1));
  }

  auto tick() {
    phase_ += inc_;

    if (phase_ >= T(1)) {
      phase_ -= T(1);
    } else if (phase_ <= T(-1)) {
      phase_ += T(1);
    }

    return phase_;
  }

private:
  T phase_ = T(0);
  T inc_ = T(0);
  const double sampleRate_;
};

} // dsp
