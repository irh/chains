#pragma once

#include <boost/hana/flatten.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/transform.hpp>

namespace chains {

template <class Processors>
class ProcessorGroup
{
public:
  ProcessorGroup(Processors processors) : processors_(processors) { init(); }

  void init()
  {
    boost::hana::for_each(processors_, [](auto& processor) { processor.init(); });
  }

  auto exposedInputs()
  {
    using namespace boost::hana;
    return flatten(
      transform(processors_, [](auto& processor) { return processor.exposedInputs(); }));
  }

protected:
  Processors processors_;
};

} // chains
