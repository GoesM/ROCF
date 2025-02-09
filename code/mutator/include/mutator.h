#ifndef THIRD_PARTY_TROOPER_MUTATOR_H_
#define THIRD_PARTY_TROOPER_MUTATOR_H_

#include <cstddef>  // import size_t
#include <cstdint>  // import uint8_t, uintptr_t
#include <cstring>
#include <array>    // import array
#include <vector>   // import vector
#include <span>     // import span
#include <map>      // import map

#include "defs.h"
#include "knobs.h"

#include <ctime>

namespace trooper {

  // A simple class representing an array of up to kMaxEntrySize bytes.
  class DictEntry {
public:
    static constexpr uint8_t kMaxEntrySize = 15;

    explicit DictEntry(ByteSpan bytes)
    : bytes_ {},   // initialize bytes_ to all zeros
    size_(bytes.size()) {
      if (size_ > kMaxEntrySize) {__builtin_trap();}
      memcpy(bytes_, bytes.data(), bytes.size());
    }
    const uint8_t * begin() const {return bytes_;}
    const uint8_t * end() const {return bytes_ + size_;}
    size_t size() const {return size_;}
    bool operator < (const DictEntry & other) const {
      return memcmp(this, &other, sizeof(*this)) < 0;
    }

private:
    // bytes_ must go first so that operator < is lexicographic.
    uint8_t bytes_[kMaxEntrySize];
    uint8_t size_;  // between kMinEntrySize and kMaxEntrySize.
  };

  // This class allows to mutate a ByteArray in different ways.
  // All mutations expect and guarantee that `data` remains non-empty
  // since there is only one possible empty input and it's uninteresting.
  //
  // This class is thread-compatible.
  // Typical usage is to have one such object per thread.
  class Mutator {
public:
    // knob_ids_ is one-one mapping to mutators_
    // knob_id is not same as its index. (see knob.h)
    static const size_t kMutatorNums_ = 7;

    // CTOR. Initializes the internal RNG with `seed` (`seed` != 0).
    // Keeps a const reference to `knobs` throughout the lifetime. ??
    Mutator(uintptr_t seed, Knobs & knobs)
      : rng_(seed), knobs_(knobs),
      knob_ids_ {
      knobs_.NewId("erase bytes"),
      knobs_.NewId("flip bit"),
      knobs_.NewId("swap bytes"),
      knobs_.NewId("change byte"),
      knobs_.NewId("overwrite from dict"),
      knobs_.NewId("insert bytes"),
      knobs_.NewId("insert from dict"),
    },
    knob_to_mutators_ {
      {knob_ids_[0], &Mutator::EraseBytes},
      {knob_ids_[1], &Mutator::FlipBit},
      {knob_ids_[2], &Mutator::SwapBytes},
      {knob_ids_[3], &Mutator::ChangeByte},
      {knob_ids_[4], &Mutator::OverwriteFromDictionary},
      {knob_ids_[5], &Mutator::InsertBytes},
      {knob_ids_[6], &Mutator::InsertFromDictionary},
    },
    strat1_(knob_ids_.data(), 1),
    strat2_(knob_ids_.data(), 5),
    strat3_(knob_ids_.data(), 7)
    {
      if (seed == 0) {
        __builtin_trap();
      }
      // some built-in dictionaries
      set_dictionary();
    }
    Mutator(Knobs & knobs)
      : Mutator(time(0), knobs)
    {
    }

    // Get to access the Knobs instance.
    // Should not add more knobs into mutator's private knobs.
    Knobs & knobs() {return knobs_;}

    // get knobs' id in this mutator.
    std::array < size_t, kMutatorNums_ > knob_ids() {
      return knob_ids_;
    }

    // add `dict_entries` to an internal dictionary
    void add_dictionary(const ByteArray & entry);

    // Type for a Mutator member-function.
    // Every mutator function takes a ByteArray& as an input, mutates it in place
    // and returns true if mutation took place. In some cases mutation may fail
    // to happen, e.g. if EraseBytes() is called on a 1-byte input.
    // Fn is test-only public.
    using Fn = bool (Mutator::*)(ByteArray &);

    using SizeSpan = std::span < const size_t >;

    // All public functions below are mutators.
    // They return true iff a mutation took place.

    // Applies some random mutation to data.
    bool Mutate(ByteArray & data);

    // Flips a random bit.
    bool FlipBit(ByteArray & data);

    // Swaps two bytes.
    bool SwapBytes(ByteArray & data);

    // Changes a random byte to a random value.
    bool ChangeByte(ByteArray & data);

    // Overwrites a random part of `data` with a random dictionary entry.
    bool OverwriteFromDictionary(ByteArray & data);

    // Inserts random bytes.
    bool InsertBytes(ByteArray & data);

    // Inserts a random dictionary entry at random position.
    bool InsertFromDictionary(ByteArray & data);

    // Erases random bytes.
    bool EraseBytes(ByteArray & data);

    // cross over mutants
    // ...

    // Set size alignment for mutants with modified sizes. Some mutators do not
    // change input size, but mutators that insert or erase bytes will produce
    // mutants with aligned sizes (if possible).
    //
    // Returns true if new size alignment was accepted. Returns false if max
    // length is not a multiple of the specified size alignment.
    bool set_size_alignment(size_t size_alignment)
    {
      if ((max_len_ != std::numeric_limits < size_t > ::max()) &&
        (max_len_ % size_alignment != 0))
      {
        return false;
      }
      size_alignment_ = size_alignment;
      return true;
    }

    // Set max length in bytes for mutants with modified sizes.
    //
    // Returns true if new max length was accepted. Returns false if specified max
    // length is not a multiple of size alignment.
    bool set_max_len(size_t max_len)
    {
      if ((max_len != std::numeric_limits < size_t > ::max()) && (max_len % size_alignment_ != 0)) {
        return false;
      }
      max_len_ = max_len;
      return true;
    }

private:
    void set_dictionary()
    {
      add_dictionary({0x00});
      add_dictionary({0xFF});
      add_dictionary({0x7F, 0xFF});
      add_dictionary({0x80, 0x00});
      add_dictionary({0xFF, 0xFF});
      add_dictionary({0x01, 0x00, 0x00});
      add_dictionary({0x7F, 0xFF, 0xFF});
      add_dictionary({0x80, 0x00, 0x00});
      add_dictionary({0xFF, 0xFF, 0xFF});
      add_dictionary({0x7F, 0xFF, 0xFF, 0xFF});
      add_dictionary({0x80, 0x00, 0x00, 0x00});
      add_dictionary({0xFF, 0xFF, 0xFF, 0xFF});
      add_dictionary({0x7F, 0xFF, 0xFF, 0xFF, 0xFF});
      add_dictionary({0x80, 0x00, 0x00, 0x00, 0x00});
      add_dictionary({0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
      add_dictionary({0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
      add_dictionary({0x80, 0x00, 0x00, 0x00, 0x00, 0x00});
      add_dictionary({0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
      add_dictionary({0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
      add_dictionary({0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
      add_dictionary({0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
      add_dictionary({0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
      add_dictionary({0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
      add_dictionary({0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
      // 32bit IEEE754
      add_dictionary({0x7F, 0x7F, 0xFF, 0xFF});   // max positive float
      add_dictionary({0x00, 0x80, 0x00, 0x00});   // min positive normalized float
      add_dictionary({0x7F, 0x80, 0x00, 0x00});   // positive infinity
      add_dictionary({0x7F, 0xC0, 0x00, 0x00});   // NaN
      // 64bit IEEE754
      add_dictionary({0x7F, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});   // max positive float
      add_dictionary({0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});   // min positive normalized float
      add_dictionary({0x7F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});   // positive infinity
      add_dictionary({0x7F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});   // NaN
      // mode value
      add_dictionary({0xAA, 0xAA});
      add_dictionary({0x55, 0x55});
      // specail value
      add_dictionary({0x0A});   // CR, \r
      add_dictionary({0x0D});   // LF, \n
      add_dictionary({0x0A, 0x0D});   // \r\n
      // unicode BOM
      add_dictionary({0xEF, 0xBB, 0xBF});   // utf-8
      add_dictionary({0xFE, 0xFF});   // utf-16
      add_dictionary({0xFF, 0xFE});   // utf-16
      // align, padding
      add_dictionary({0xAB, 0xAB, 0xAB, 0xAB});
      add_dictionary({0xCD, 0xCD, 0xCD, 0xCD});
    }

    // Given a current size and a number of bytes to add, returns the number of
    // bytes that should be added for the resulting size to be properly aligned.
    //
    // If the original to_add would result in an unaligned input size, we round up
    // to the next larger aligned size.
    //
    // This function respects `max_len_` and will return 0 if curr_size is already
    // greater than or equal to `max_len_`.
    size_t RoundUpToAdd(size_t curr_size, size_t to_add);

    // Given a current size and a number of bytes to remove, returns the number
    // of bytes that should be removed for the resulting size to be property
    // aligned.
    //
    // If the original to_remove would result in an unaligned input size, we
    // round down to the next smaller aligned size.
    //
    // However, we never return a number of bytes to remove that would result in
    // a 0 size. In this case, the resulting size will be the smaller of
    // curr_size and size_alignment_.
    //
    // This function respects `max_len_` and may return a larger number
    // necessary to get the mutant's size to below `max_len_`.
    size_t RoundDownToRemove(size_t curr_size, size_t to_remove);

    // Size alignment in bytes to generate mutants.
    //
    // For example, if size_alignment_ is 1, generated mutants can have any
    // number of bytes. If size_alignment_ is 4, generated mutants will have
    // sizes that are 4-byte aligned.
    size_t size_alignment_ = 1;

    // Max length of a generated mutant in bytes.
    // initialize with max length of size_t
    size_t max_len_ = std::numeric_limits < size_t > ::max();

    Rng rng_;
    Knobs & knobs_;
    const std::array < size_t, kMutatorNums_ > knob_ids_;
    const std::map < size_t, Fn > knob_to_mutators_;

    const std::span < const size_t > strat1_; // decrease size
    const std::span < const size_t > strat2_; // decrease/keep
    const std::span < const size_t > strat3_; // decrease/keep/increase
    std::vector < DictEntry > dictionary_;
  };
}  // namespace trooper

#endif  // THIRD_PARTY_TROOPER_MUTATOR_H_
