#include "chains/groups/parallel.hpp"
#include "chains/groups/recursive.hpp"
#include "chains/groups/serial.hpp"
#include "chains/groups/split.hpp"
#include "chains/module.hpp"
#include "chains/modules/crossfade.hpp"
#include "chains/modules/gain.hpp"

#include <cstdlib>

int main()
{
  using namespace chains;

  const auto chain =
    serial(split(module<Gain>(Value<gain::Gain>(1)), module<Gain>(Value<gain::Gain>(2))),
           module<Crossfade, Expose<crossfade::Fade>>(Value<crossfade::Fade>(0.5)));

  auto processor = chain.makeProcessor<double>(44100);
  auto fade = boost::hana::at_c<0>(processor.exposedInputs());

  fade->setValue(rand() / RAND_MAX);

  return processor.tick(rand() / RAND_MAX);
}
