#include <boost/hana/any_of.hpp>
#include <boost/hana/fold.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/tuple.hpp>

#include <catch/single_include/catch.hpp>

#include <array>
#include <cassert>
#include <iostream>

namespace chains {

namespace hana = boost::hana;

template<class Modules>
auto makeProcessors(const Modules& modules)
{
  return hana::transform(
    modules, [](const auto& module) { return module.makeProcessor(); });
}

template <template <class> class ProcessorHost, class... Modules>
struct ModuleHost {
  ModuleHost(Modules... modules)
    : modules_(modules...)
  {}

  auto makeProcessor() {
    using Processor = ProcessorHost<decltype(makeProcessors(modules_))>;
    return Processor{makeProcessors(modules_)};
  }

protected:
  hana::tuple<Modules...> modules_;
};

template <typename Processors>
struct ProcessorHost {
  ProcessorHost(Processors processors)
    : processors_(processors)
  {}

  void setParameter(const int processorId, const int parameterIndex, const double value)
  {
    hana::any_of(processors_,
      [=] (auto& processor) {
        if (processor.id() == processorId) {
          processor.parameter(parameterIndex).setValue(value);
          return true;
        }
        return false;
      });
  }

protected:
  Processors processors_;
};

template <class Processors>
struct SerialProcessor : ProcessorHost<Processors> {
  using ProcessorHost<Processors>::ProcessorHost;

  template<class T>
  auto tick(const T& in) {
    return hana::fold(
      this->processors_, in,
      [](const T& in, auto&& processor) { return processor.tick(in); });
  }
};

template <class... Modules>
auto serial2(Modules... modules) {
  return ModuleHost<SerialProcessor, Modules...>(modules...);
}

template <class Processors>
struct ParallelProcessor : ProcessorHost<Processors> {
  using ProcessorHost<Processors>::ProcessorHost;

  template<class T>
  auto tick(const T& in) {
    return hana::fold(
      this->processors_, T(0),
      [&in](const T& result, auto&& processor) { return result + processor.tick(in); });
  }
};

template <class... Modules>
auto parallel2(Modules... modules) {
  return ModuleHost<ParallelProcessor, Modules...>(modules...);
}

} // chains


struct Parameter {
  constexpr Parameter(const char* name)
    : name_(name)
  {}

  constexpr auto defaultValue(const double value) {
    defaultValue_ = value;
    return *this;
  }

  auto defaultValue() const { return defaultValue_; }

  struct Input {
    Input() = default;
    explicit Input(const double value) : value_(value) {}

    auto value() const { return value_; }
    void setValue(const double value) { value_ = value; }

  private:
    double value_ = 0.0;
  };

private:
  const char* const name_;
  double defaultValue_ = 0.0;
};


template<class... Args>
constexpr auto declareParameters(Parameter&& first, Args&&... rest) {
  return std::array<Parameter, sizeof...(Args) + 1>{
    {std::forward<Parameter>(first)}, std::forward<Args>(rest)...
  };
}

template<class T>
struct Module {
  int id() const { return id_; }
  auto id(const int id) {
    id_ = id;
    return moduleRef();
  }

  static constexpr auto parameterCount() { return T::Parameters::kCount; }

  struct Processor {
    Processor() {
      using namespace std;

      const auto parameters = T::parameters();
      static_assert(parameters.size() == parameterCount());

      transform(begin(parameters), end(parameters), begin(parameterInputs_),
        [] (const auto& parameter)
        {
          return Parameter::Input{parameter.defaultValue()};
        });
    }

    template <int index>
    const auto& parameter() const {
      static_assert(index < parameterCount(), "Index is out of range");
      return parameterInputs_[index];
    }

    auto& parameter(const int index) {
      assert(index < parameterCount());
      return parameterInputs_[index];
    }

    int id() const { return id_; }
    void id(const int id) { id_ = id; }

  private:
    int id_ = -1;
    std::array<Parameter::Input, parameterCount()> parameterInputs_;
  };

  auto makeProcessor() const {
    auto result = typename T::Processor{};
    result.id(id_);
    return result;
  }

private:
  auto moduleRef() {
    return *static_cast<T*>(this);
  }

  int id_ = -1;
};

struct AddModule : public Module<AddModule> {
  enum Parameters {
    kAmount,
    kCount
  };

  static constexpr auto parameters() {
    return declareParameters(
      Parameter("Amount")
       .defaultValue(0.0)
      );
  }

  struct Processor : public Module<AddModule>::Processor {
    Processor() = default;

    auto tick(const double& in) {
      return in + parameter<kAmount>().value();
    }
  };
};

struct MultiplyModule : public Module<MultiplyModule> {
  enum Parameters {
    kAmount,
    kCount
  };

  static constexpr auto parameters() {
    return declareParameters(
      Parameter("Amount")
       .defaultValue(0.0)
      );
  }

  struct Processor : public Module<MultiplyModule>::Processor {
    Processor() = default;

    auto tick(const double& in) {
      return in * parameter<kAmount>().value();
    }
  };
};

TEST_CASE("parameters")
{
  enum {
    kAdd1,
    kAdd2,
    kMultiply
  };

  SECTION("one")
  {
    auto chain = chains::serial2(AddModule{}.id(kAdd1)).makeProcessor();
    chain.setParameter(kAdd1, AddModule::kAmount, 1.0);
    CHECK(chain.tick(4) == 5.0);
  }

  SECTION("two in serial")
  {
    auto chain = chains::serial2(AddModule{}.id(kAdd1), MultiplyModule{}.id(kMultiply)).makeProcessor();

    chain.setParameter(kAdd1, AddModule::kAmount, 20.0);
    chain.setParameter(kMultiply, MultiplyModule::kAmount, 3.0);

    CHECK(chain.tick(2) == 66.0);
  }

  SECTION("two in parallel")
  {
    auto chain = chains::parallel2(AddModule{}.id(kAdd1), MultiplyModule{}.id(kMultiply)).makeProcessor();

    chain.setParameter(kAdd1, AddModule::kAmount, 20.0);
    chain.setParameter(kMultiply, MultiplyModule::kAmount, 3.0);

    CHECK(chain.tick(2) == 28.0);
  }
}
