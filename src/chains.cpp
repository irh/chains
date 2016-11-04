#include "chains/groups/parallel.hpp"
#include "chains/groups/recursive.hpp"
#include "chains/groups/serial.hpp"
#include "chains/groups/split.hpp"
#include "chains/modules/accumulator.hpp"
#include "chains/modules/crossfade.hpp"
#include "chains/modules/delay.hpp"
#include "chains/modules/gain.hpp"
#include "chains/modules/ones.hpp"
#include "chains/modules/phasor.hpp"
#include "chains/modules/probe.hpp"
#include "chains/modules/wire.hpp"

#include <catch/single_include/catch.hpp>


TEST_CASE("Wrapper")
{
  using namespace chains;
  namespace hana = boost::hana;

  SECTION("Foo")
  {
    using namespace accumulator;

    const auto chain = serial(module<Accumulator, Expose<Amount, Wrap>>("A"),
                              module<Accumulator, Expose<Amount>>("B", Value<Wrap>{2.0}),
                              module<Accumulator>(Value<Amount>{1.0}, Value<Wrap>{4.0}));

    const auto paramDefs = chain.exposedParameters();
    CHECK(hana::at_c<0>(paramDefs).name() == "A Amount");
    CHECK(hana::at_c<1>(paramDefs).name() == "A Wrap");
    CHECK(hana::at_c<2>(paramDefs).name() == "B Amount");

    auto processor = chain.makeProcessor<double>(44100);
    auto inputs = processor.exposedInputs();

    hana::at_c<0>(inputs)->setValue(0.5);
    hana::at_c<1>(inputs)->setValue(2.0);
    hana::at_c<2>(inputs)->setValue(0.5);

    CHECK(processor.tick(1.0) == 0.25);
    CHECK(processor.tick(1.0) == 1.0);
    CHECK(processor.tick(1.0) == 2.5);
    CHECK(processor.tick(1.0) == 0.0);
  }

  SECTION("Generator")
  {
    const auto chain =
      serial(module<Ones>(), module<Accumulator>(Value<accumulator::Wrap>{4}),
             module<Gain>(Value<gain::Gain>{0.25}));

    auto processor = chain.makeProcessor<double>(48e3);

    CHECK(processor.tick() == 0.25);
    CHECK(processor.tick() == 0.5);
    CHECK(processor.tick() == 0.75);
    CHECK(processor.tick() == 0.0);
  }

  SECTION("Phasor")
  {
    const auto chain = serial(module<Phasor, Expose<phasor::Frequency>>());

    auto processor = chain.makeProcessor<double>(4);

    auto inputs = processor.exposedInputs();
    auto frequency = hana::at_c<0>(inputs);

    CHECK(processor.tick() == 0.25);
    CHECK(processor.tick() == 0.5);
    CHECK(processor.tick() == 0.75);
    CHECK(processor.tick() == 0.0);

    frequency->setValue(2);

    CHECK(processor.tick() == 0.5);
    CHECK(processor.tick() == 0.0);
  }

  SECTION("Parallel")
  {
    const auto chain = parallel(module<Gain>("1", Value<gain::Gain>{0.5}),
                                module<Gain>("2", Value<gain::Gain>{2.0}));

    auto processor = chain.makeProcessor<double>(96e3);

    CHECK(processor.tick(1) == 2.5);
    CHECK(processor.tick(2) == 5.0);
  }

  SECTION("Crossfade")
  {
    const auto chain = serial(
      split(module<Gain>(Value<gain::Gain>(1)), module<Gain>(Value<gain::Gain>(2))),
      module<Crossfade, Expose<crossfade::Fade>>(Value<crossfade::Fade>(0.5)));

    auto processor = chain.makeProcessor<double>(96e3);

    auto inputs = processor.exposedInputs();
    auto fade = hana::at_c<0>(inputs);

    CHECK(processor.tick(1) == 1.5);
    CHECK(processor.tick(2) == 3.0);

    fade->setValue(0.0);

    CHECK(processor.tick(1) == 1.0);
    CHECK(processor.tick(2) == 2.0);

    fade->setValue(1.0);

    CHECK(processor.tick(1) == 2.0);
    CHECK(processor.tick(2) == 4.0);
  }

  SECTION("Recursive")
  {
    const auto chain = recursive(
      module<Gain>(Value<gain::Gain>{2.0}), module<Gain>(Value<gain::Gain>{0.25}));

    auto processor = chain.makeProcessor<double>(48e3);

    CHECK(processor.tick(1) == 2.0);
    CHECK(processor.tick(0) == 1.0);
    CHECK(processor.tick(0) == 0.5);
    CHECK(processor.tick(0) == 0.25);
  }

  SECTION("Named")
  {
    const auto phasor = serial(module<Phasor, Expose<phasor::Frequency>>(),
                               module<Gain, Expose<gain::Gain>>("Gain"));

    const auto chain = parallel(phasor.named("Phasor A"), phasor.named("Phasor B"));

    const auto paramDefs = chain.exposedParameters();
    CHECK(hana::at_c<0>(paramDefs).name() == "Phasor A Frequency");
    CHECK(hana::at_c<1>(paramDefs).name() == "Phasor A Gain");
    CHECK(hana::at_c<2>(paramDefs).name() == "Phasor B Frequency");
    CHECK(hana::at_c<3>(paramDefs).name() == "Phasor B Gain");
  }

  SECTION("Delay")
  {
    const auto chain = serial(module<Delay, Expose<delay::Length>>());

    auto processor = chain.makeProcessor<double>(1000);
    auto inputs = processor.exposedInputs();
    auto length = hana::at_c<0>(inputs);

    length->setValue(0);
    CHECK(processor.tick(1.0) == 1.0);
    CHECK(processor.tick(0.5) == 0.5);
    CHECK(processor.tick(0.0) == 0.0);

    length->setValue(1);
    CHECK(processor.tick(0.0) == 0.0);
    CHECK(processor.tick(0.0) == 0.0);
    CHECK(processor.tick(0.0) == 0.0);
    CHECK(processor.tick(0.0) == 0.0);

    CHECK(processor.tick(1.0) == 0.0);
    CHECK(processor.tick(0.5) == 1.0);
    CHECK(processor.tick(0.0) == 0.5);
    CHECK(processor.tick(0.0) == 0.0);

    length->setValue(2);
    CHECK(processor.tick(1.0) == 0.0);
    CHECK(processor.tick(0.5) == 0.0);
    CHECK(processor.tick(0.0) == 1.0);
    CHECK(processor.tick(0.0) == 0.5);
    CHECK(processor.tick(0.0) == 0.0);
  }

  SECTION("Synth")
  {
    // const auto osc = serial(
    //   module<oscillator::Module>("", Expose<oscillator::Note, oscillator::Shape>),
    //   module<gain::Module>);

    // const auto oscillatorSection =
    //   parallel(named(osc, "Osc A"), named(osc, "Osc B"), named(osc, "Osc C"));
  }
}
