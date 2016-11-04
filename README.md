# Chains

This is a work-in-progress experimental C++ library for declaring
high-level signal chains in such a way that we can take advantage of some
compile-time template trickery to achieve things like:
- Knowledge at compile time of the number of exposed parameters in a signal
chain
- The ability to replace variable parameters with constants if they aren't
exposed to the user
- An expressive (for C++) syntax for declaring signal chains, e.g.

```c++
const auto phasor = serial(module<Phasor, Expose<phasor::Frequency>>(),
                           module<Gain, Expose<gain::Gain>>("Gain"));

const auto chain = parallel(phasor.named("Phasor A"), phasor.named("Phasor B"));

const auto paramDefs = chain.exposedParameters();
CHECK(hana::at_c<0>(paramDefs).name() == "Phasor A Frequency");
CHECK(hana::at_c<1>(paramDefs).name() == "Phasor A Gain");
CHECK(hana::at_c<2>(paramDefs).name() == "Phasor B Frequency");
CHECK(hana::at_c<3>(paramDefs).name() == "Phasor B Gain");

auto processor = chain.makeProcessor<double>(48e3);
auto phasorAFrequency = hana::at_c<0>(processor.exposedInputs());
phasorAFrequency->setValue(1.0);

processor.tick();
```

## Notes
- This library is experimental, so please don't expect it to be useful or
working correctly in any way.
- Catch is used for unit testing and is included as a git submodule.
- Boost.hana is used in the library, so Boost v.1.61 should be available on your
system.
- I've only tested compiling this library so far on a Mac, but I expect it
should work on Linux without too much fuss. Some cmake tweaking would be needed
to get things building on Windows, but with a recent version of Visual Studio
it should be possible.

## TODO
Some features I'd like to explore sometime in the future:
- Some useful DSP building blocks
- A modulation system for arbitrary connections between mod sources and targets
- Polyphonic chains for building instruments
- Automatic UI generation based on exposed parameters
- Experiment with operator overloading to provide an alternative chain
declaration syntax

