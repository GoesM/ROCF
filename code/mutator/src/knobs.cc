#include "./knobs.h"

#include <cstdio>
#include <string_view>

namespace trooper {
  // notice that knobs is decoupled from the mutation method
  // knobs' value is decoupled from knobs_name (corresponding to a mehod)

  size_t Knobs::NewId(std::string_view knob_name)
  {
    if (next_id_ >= kNumKnobs) {
      // run out the ids
      fprintf(stderr, "knobs::NewId: no more IDS left, aborting\n");
      __builtin_trap();
    }
    knob_names_[next_id_] = knob_name;
    return next_id_++;
  }

  size_t Knobs::Choose(std::span < const size_t > knob_ids, uint64_t random) const
  {
    size_t sum = 0;
    for (auto knob_id : knob_ids) {
      sum += Value(knob_id);
    }
    if (sum == 0) {
          // printf("[debug]: choose id: %d \n",knob_ids[random % knob_ids.size()]);
      return knob_ids[random % knob_ids.size()];
    }
    random %= sum;
    size_t partial_sum = 0;
    size_t idx = 0;
    for (auto knob_id : knob_ids) {
      partial_sum += Value(knob_id);
      if (partial_sum > random) {
            // printf("[debug]: choose id: %d \n",knob_ids[idx]);
        return knob_ids[idx];
      }
      ++idx;
    }
    __builtin_unreachable();
  }

  // see doi.org/10.48550/arXiv.1109.3627
  // unstable when using low values of knobs
  size_t Knobs::Choose2(std::span < const size_t > knob_ids, uint64_t random) const
  {
    size_t n = knob_ids.size();
    uint64_t r = LCG(random);
    size_t knob_id = knob_ids[r % n];
    for (size_t i = 0; i < n; ++i) {
      if (TossUp(knob_id, r >> 16)) {return knob_id;}
      r = LCG(r); // iterate lcg
      knob_id = knob_ids[r % n];
    }
    // worst lower bound, just return a random knob
    return knob_id;
  }

  // decide wether to choose knob, by the probability of knob/max
  // map knob and random to [0, max), check if `knob >= random`
  bool Knobs::TossUp(size_t knob_id, uint64_t random) const
  {
    uint8_t knob = Value(knob_id);
    // always true if max is 0, using short-circuit evaluation
    // to prevent division by 0 in `random%max`
    // notice that `knob_max_` is class member
    return !knob_max_ || knob >= random % knob_max_;
  }
} // namespace trooper
