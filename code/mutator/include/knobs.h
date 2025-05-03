#ifndef THIRD_PARTY_TROOPER_KNOBS_H_
#define THIRD_PARTY_TROOPER_KNOBS_H_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <string_view>
#include <span>


namespace trooper {

  class Knobs {
public:
    static constexpr size_t kNumKnobs = 50;

    // associate a `knob_name` with new `knob_id`
    // Must be called at the process startup (assign the result to a global):
    //   static const KnobId knob_weight_of_foo = Knobs::NewId("weight_of_foo");
    // Will trap if runs out of IDs.
    size_t NewId(std::string_view knob_name);

    // Returns the name associated with `knob_id`.
    std::string_view Name(size_t knob_id) const
    {
      return knob_names_[knob_id];
    }

    // Sets all knobs to the same value `value`.
    void Set(uint8_t value)
    {
      for (auto & knob : knobs_) {
        knob = value;
      }
      knob_max_ = value;
    }

    // Sets the knobs to values from `values`. If `values.size() < kNumKnobs`,
    // only the first `values.size()` values will be set.
    void Set(std::span < const uint8_t > values)
    {
      size_t n = std::min(kNumKnobs, values.size());
      for (size_t i = 0; i < n; ++i) {
        knobs_[i] = values[i];
        if (values[i] > knob_max_) {
          knob_max_ = values[i];
        }
      }
    }

    // set value of knob with this id
    void Set(uint8_t value, size_t knob_id)
    {
      knobs_[knob_id] = value;
      if (value > knob_max_) {
        knob_max_ = value;
      }
    }

    // Returns the value associated with `knob_id`.
    uint8_t Value(size_t knob_id) const
    {
      if (knob_id >= kNumKnobs) {
        __builtin_trap();
      }
      return knobs_[knob_id];
    }

    // Calls `callback(Name, Value)` for every KnobId created by NewId().
    void ForEachKnob(
      const std::function < void(std::string_view, uint8_t) > & callback)
    const
    {
      for (size_t i = 0; i < next_id_; ++i) {
        callback(Name(i), Value(i));
      }
    }

    // return numbers of current knobs
    size_t next_id() {return next_id_;}

    // Uses knob values associated with knob_ids as probability weights for
    // respective choices. E.g. if knobs.Value(knobA) == 100 and
    // knobs.Value(knobB) == 10, then Choose<...>({knobA, knobB}, {A, B}, rng())
    // is approximately 10x more likely to return A than B.
    //
    // If all knob values are zero, behaves as if they were all 1.
    size_t Choose(std::span < const size_t > knob_ids, uint64_t random) const;

    size_t Choose2(std::span < const size_t > knob_ids, uint64_t random) const;

    // Chooses between two strategies, i.e. returns true or false.
    // Treats the value of the knob associated with `knob_id` as signed integer.
    // If knob == -128, returns false. If knob == 127 returns true.
    // For other values, returns randomly true of false, with higher probability
    // of true for higher values of knob.
    // If knob == 0, returns true with a ~ 50% chance.
    // `random` is a random number used to produce random choice.
    bool TossUp(size_t knob_id, uint64_t random) const;

private:
    // Linear Congruent Generator, fast derived prng using
    // Park Miller Algorithm: a * 16807 % (2^31-1)
    // No internal state, which means the same `x` always results
    // in same rand output. External iteration is required
    uint64_t LCG(uint64_t x) const
    {
      x *= 16807;
      x = (x & 0x7fffffff) + (x >> 31);
      x = (x & 0x7fffffff) + (x >> 31);
      return x;
    }

    size_t next_id_ = 0;
    std::string_view knob_names_[kNumKnobs];
    uint8_t knobs_[kNumKnobs] = {};
    uint8_t knob_max_ = 0;
  };

} // namespace trooper

#endif // THIRD_PARTY_TROOPER_KNOBS_H_
