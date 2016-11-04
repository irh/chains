#include <catch/single_include/catch.hpp>

#include <boost/hana/fold.hpp>
#include <boost/hana/basic_tuple.hpp>

#include <cassert>

namespace chains {


namespace hana = boost::hana;


template<class... Nodes>
struct Serial {
  Serial(Nodes... nodes) : nodes_(nodes...) {}

  template<class T>
  auto process(const T& in) {
    return hana::fold(
      nodes_, in,
      [](const T& in, auto& node) { return node.process(in); });
  }

private:
  hana::basic_tuple<Nodes...> nodes_;
};

template <class... Nodes>
auto serial(Nodes... args) {
  return Serial<Nodes...>(args...);
}
template<class... Nodes>
struct Parallel {
  Parallel(Nodes... nodes)
    : nodes_(nodes...)
  {}

  template<class T>
  auto process(const T& in) {
    return hana::fold(
      nodes_, T(0),
      [&in] (const T& result, auto& node) {
        return result + node.process(in);
      });
  }

private:
  hana::basic_tuple<Nodes...> nodes_;
};

template <class... Nodes>
auto parallel(Nodes... args) {
  return Parallel<Nodes...>(args...);
}

template<class Forward, class Back, class T>
struct Recursive {
  Recursive(Forward forward, Back back, T initialFeedback)
    : forward_(forward)
    , back_(back)
    , previous_(initialFeedback)
  {}

  auto process(const T& in) {
    const auto forward = forward_.process(in + previous_);
    previous_ = back_.process(forward);
    return forward;
  }

private:
  Forward forward_;
  Back back_;
  T previous_;
};

template<class Forward, class Back, class T>
auto recursive(Forward&& forward, Back&& back, T&& initialFeedback) {
  return Recursive<Forward, Back, T>(forward, back, initialFeedback);
}


namespace detail {
  template<int N, class Tuple, class T>
  T process(const T& in, Tuple& tuple) {
    return std::get<N>(tuple).process(in);
  }

  template<class Tuple, class T, std::size_t... Is>
  T process(const T& in, Tuple& tuple, std::size_t index, std::index_sequence<Is...>) {
    using ProcessFunction = T(const T&, Tuple&);

    static constexpr ProcessFunction* processFunctions[] = { &process<Is, Tuple, T>... };

    return processFunctions[index](in, tuple);
  }
}

template<class... Nodes>
struct Select : public std::tuple<Nodes...> {
  using Tuple = std::tuple<Nodes...>;

  Select(Nodes&&... nodes)
    : Tuple(nodes...)
  {}

  template<class T>
  auto process(const T& in) {
    return detail::process(in, *this, index_, std::index_sequence_for<Nodes...>{});
  }

  void select(const int index) {
    assert(index >= 0 && index < std::tuple_size<Tuple>::value);
    index_ = index;
  }

private:
  int index_ = 0;
};


template <class... Nodes>
auto select(Nodes&&... args) {
  return Select<std::decay_t<Nodes>...>(std::forward<Nodes>(args)...);
}

} // chains

template <typename T>
struct Add {
  Add(const T& amount)
    : amount_(amount)
  {}

  auto process(const T& in) {
    return in + amount_;
  }

private:
  T amount_;
};

template<class T>
auto add(const T& n) {
  return Add<T>{n};
}

template <typename T>
struct Multiply {
  Multiply(const T& amount)
    : amount_(amount)
  {}

  auto process(const T& in) {
    return in * amount_;
  }

private:
  T amount_;
};

template<class T>
auto multiply(const T& n) {
  return Multiply<T>{n};
}


TEST_CASE("chains") {
  using namespace chains;

  SECTION("serial") {
    auto chain = serial(multiply(5), add(42), multiply(10));
    CHECK(chain.process(3) == (3 * 5 + 42) * 10);
  }

  SECTION("parallel") {
    auto chain = parallel(add(5), add(42), multiply(2));
    CHECK(chain.process(10) == 15 + 52 + 20);
  }

  SECTION("parallel+serial") {
    auto chain = parallel(
      serial(add(1), multiply(3)),
      serial(multiply(9), add(5), multiply(-1)));
    CHECK(chain.process(2) == (2 + 1) * 3 + (2 * 9 + 5) * -1);
  }

  SECTION("recursive") {
    auto chain = recursive(add(0), add(-1), 0);
    CHECK(chain.process(3) == 3);
    CHECK(chain.process(0) == 2);
    CHECK(chain.process(0) == 1);
    CHECK(chain.process(0) == 0);
  }

  SECTION("select") {
    auto chain = select(multiply(2), add(5), serial(add(3), multiply(2)));
    chain.select(1);
    CHECK(chain.process(1) == 6);
    chain.select(0);
    CHECK(chain.process(1) == 2);
    chain.select(2);
    CHECK(chain.process(1) == 8);
  }
}

