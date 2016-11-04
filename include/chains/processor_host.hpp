#pragma once

#include "chains/support/can_apply.hpp"

#include <boost/hana/at_key.hpp>
#include <boost/hana/tuple.hpp>

namespace chains {

namespace detail {

template <class T>
using CheckForInit = decltype(std::declval<T>().init());

template <class T>
constexpr bool hasInitMethod = canApply<CheckForInit, T>::value;

} // detail

// No-op for processors without an init method
template <class Processor, class = void>
struct InitializeProcessor
{
  static void init(Processor&) {}
};

// Call init on processor, if it has an init method
template <class Processor>
struct InitializeProcessor<Processor, std::enable_if_t<detail::hasInitMethod<Processor>>>
{
  static void init(Processor& processor) { processor.init(); }
};

// A wrapper that provides a standard interface to processors
template <class Processor, class Inputs, class... Exposed>
class ProcessorHost
{
  Processor processor_;

public:
  ProcessorHost(const Inputs& inputs, const double sampleRate)
    : processor_(inputs, sampleRate)
  {
  }

  template <class T>
  auto tick(const T& in = T(0))
  {
    return processor_.tick(in);
  }

  auto exposedInputs()
  {
    using namespace boost::hana;
    return make_tuple(&(processor_.inputs_[type_c<typename Exposed::Traits>])...);
  }

  void init() { InitializeProcessor<Processor>::init(processor_); }
};

template <class ParameterTraits, class Inputs>
auto getValue(const Inputs& inputs)
{
  return inputs[boost::hana::type_c<ParameterTraits>].value();
}

template <class ParameterTraits, class Inputs, class Callback>
void setCallback(Inputs& inputs, Callback callback)
{
  return inputs[boost::hana::type_c<ParameterTraits>].setCallback(callback);
}

} // chains
