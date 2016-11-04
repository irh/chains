#pragma once

#include "chains/support/estd.hpp"

#include <functional>
#include <iostream>
#include <string>

namespace chains {

using ValueCallback = std::function<void(double)>;

// Tag for a parameter that should create an InputWithCallback input when exposed
struct CallbackParameter
{
};

class CallbackInput
{
  double value_ = 0.0;
  ValueCallback callback_;

public:
  explicit CallbackInput(const double value) : value_(value) {}

  auto value() const { return value_; }

  void setValue(const double value)
  {
    const auto oldValue = value_;
    value_ = value;
    if (oldValue != value) {
      callback_(value);
    }
  }

  void setCallback(ValueCallback callback)
  {
    callback_ = callback;
    callback_(value_);
  }
};

class Input
{
  double value_ = 0.0;

public:
  explicit Input(const double value) : value_(value) {}

  auto value() const { return value_; }
  void setValue(const double value) { value_ = value; }
};

class Constant
{
  const double value_;

public:
  explicit Constant(const double value) : value_(value) {}

  auto value() const { return value_; }

  void setCallback(ValueCallback&& callback) { callback(value_); }
};


template <class Traits>
using EnableIfCallbackParameter =
  std::enable_if_t<estd::is_base_of_v<CallbackParameter, Traits>>;

template <class Traits, bool Exposed, class Enable = void>
struct InputSelector
{
  using type = std::conditional_t<Exposed, Input, Constant>;
};

template <class Traits>
struct InputSelector<Traits, true, EnableIfCallbackParameter<Traits>>
{
  using type = CallbackInput;
};


// Parameter provides parameter info to modules based on the provided traits
template <class TTraits>
class Parameter
{
  double defaultValue_;
  const char* moduleName_;

public:
  using Traits = TTraits;

  explicit Parameter(double defaultValue = Traits::defaultValue())
    : defaultValue_(defaultValue), moduleName_(nullptr)
  {
  }

  explicit Parameter(const char* moduleName)
    : defaultValue_(Traits::defaultValue()), moduleName_(moduleName)
  {
  }

  auto name() const
  {
    if (moduleName_) {
      return std::string{moduleName_} + ' ' + Traits::name();
    } else {
      return std::string{Traits::name()};
    }
  }

  auto minimumValue() const { return Traits::minimumValue(); }
  auto maximumValue() const { return Traits::maximumValue(); }
  auto defaultValue() const { return defaultValue_; }

  template <bool exposed>
  auto makeInput() const
  {
    return typename InputSelector<Traits, exposed>::type{defaultValue()};
  }
};

template <class... Traits>
using ParameterTraits = std::tuple<Traits...>;

} // chains
