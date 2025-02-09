#include "./knobs.h"
#include "./defs.h"
#include <chrono>
#include <array>
#include <iostream>
#include <span>

namespace trooper {
  void Test()
  {
    // using Unix timestamp as rng seed
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    Knobs knobs;
    knobs.NewId("knob1");
    knobs.NewId("knob2");
    knobs.NewId("knob3");
    knobs.NewId("knob4");
    knobs.NewId("knob5");
    std::array < uint8_t, 5 > knob_values = {133, 13, 8, 25, 255, };
    std::array < size_t, 5 > knob_ids = {0, 1, 2, 3, 4};
    knobs.Set(knob_values);

    // test for probabilistic selection
    const size_t N = 10000;
    std::vector < size_t > counts(knob_values.size(), 0);

    for (size_t i = 0; i < N; ++i) {
      Rng rng(seed + i);
      size_t chosen_knob = knobs.Choose2(knob_ids, rng());
      counts[chosen_knob]++;
    }
    std::cout << "test probabilistic choose: " << std::endl;
    for (size_t i = 0; i < counts.size(); ++i) {
      std::cout << "  " << "Knob " << i << " chosen ratio: " << static_cast < double >
        (counts[i]) / N << std::endl;
    }
  }
} // namespace trooper

int main()
{
  trooper::Test();
  return 0;
}
