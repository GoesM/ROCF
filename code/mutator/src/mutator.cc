#include "mutator.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <span>

#include <iostream>

#include "defs.h"
#include "knobs.h"

namespace trooper {

  // rng * knobs: [0, 0, 0, 0, 0, 0] -->
  // * same size mutate: filp bit, swap bytes, change byte,
  // overwrite from dictionary.
  // * decrease size mutate: erase bytes.
  // * increase size mutate: insert bytes, insert from dictionary.

  bool Mutator::Mutate(ByteArray & data)
  {
    // Individual mutator may fail to mutate and return false.
    // So we iterate a few times and expect one of the mutations will succeed.
    for (int iter = 0; iter < 15; iter++) {
      Fn mutator = nullptr;
      size_t knob_id = knobs_.kNumKnobs; // just an invalid num
      if (data.size() > max_len_) {
        // only decrease size mutation is acceptable
        knob_id = knobs_.Choose(strat1_, rng_());
      } else if (data.size() == max_len_) {
        // decrease, and same size mutation
        knob_id = knobs_.Choose(strat2_, rng_());
      } else {
        // decrease, same, increase size mutation
        knob_id = knobs_.Choose(strat3_, rng_());
      }
      mutator = knob_to_mutators_.at(knob_id);
      if ((this->*mutator)(data)) {
        return true;
      }
    }
    return false;
  }

  bool Mutator::FlipBit(ByteArray & data)
  {
    if (!data.size()) {
      return false;
    }
    uintptr_t random = rng_();
    size_t bit_idx = random % (data.size() * 8);
    size_t byte_idx = bit_idx / 8;
    bit_idx %= 8;
    uint8_t mask = 1 << bit_idx;
    data[byte_idx] ^= mask;
    return true;
  }

  bool Mutator::SwapBytes(ByteArray & data)
  {
    if (!data.size()) {
      return false;
    }
    size_t idx1 = rng_() % data.size();
    size_t idx2 = rng_() % data.size();
    std::swap(data[idx1], data[idx2]);
    return true;
  }

  bool Mutator::ChangeByte(ByteArray & data)
  {
    if (!data.size()) {
      return false;
    }
    size_t idx = rng_() % data.size();
    data[idx] = rng_();
    return true;
  }

  bool Mutator::InsertBytes(ByteArray & data)
  {
    // Don't insert too many bytes at once.
    const size_t kMaxInsertSize = 20;
    size_t num_new_bytes = rng_() % kMaxInsertSize + 1;
    num_new_bytes = RoundUpToAdd(data.size(), num_new_bytes);
    if (num_new_bytes > kMaxInsertSize) {
      num_new_bytes -= size_alignment_;
    }
    // There are N+1 positions to insert something into an array of N.
    size_t pos = rng_() % (data.size() + 1);
    // Fixed array to avoid memory allocation.
    std::array < uint8_t, kMaxInsertSize > new_bytes;
    for (size_t i = 0; i < num_new_bytes; i++) {
      new_bytes[i] = rng_();
    }
    data.insert(
      data.begin() + pos, new_bytes.begin(),
      new_bytes.begin() + num_new_bytes);
    return true;
  }

  bool Mutator::EraseBytes(ByteArray & data)
  {
    if (data.size() <= size_alignment_) {
      return false;
    }
    // Ok to erase a sizable chunk since small inputs are good (if they
    // produce good features).
    size_t num_bytes_to_erase = rng_() % (data.size() / 2) + 1;
    num_bytes_to_erase = RoundDownToRemove(data.size(), num_bytes_to_erase);
    if (num_bytes_to_erase == 0) {
      return false;
    }
    size_t pos = rng_() % (data.size() - num_bytes_to_erase + 1);
    data.erase(data.begin() + pos, data.begin() + pos + num_bytes_to_erase);
    return true;
  }

  bool Mutator::OverwriteFromDictionary(ByteArray & data)
  {
    if (dictionary_.empty()) {
      return false;
    }
    size_t dict_entry_idx = rng_() % dictionary_.size();
    const auto & dic_entry = dictionary_[dict_entry_idx];
    if (dic_entry.size() > data.size()) {
      return false;
    }
    size_t overwrite_pos = rng_() % (data.size() - dic_entry.size() + 1);
    std::copy(dic_entry.begin(), dic_entry.end(), data.begin() + overwrite_pos);
    return true;
  }

  bool Mutator::InsertFromDictionary(ByteArray & data)
  {
    if (dictionary_.empty()) {
      return false;
    }
    size_t dict_entry_idx = rng_() % dictionary_.size();
    const auto & dict_entry = dictionary_[dict_entry_idx];
    // There are N+1 positions to insert something into an array of N.
    size_t pos = rng_() % (data.size() + 1);
    data.insert(data.begin() + pos, dict_entry.begin(), dict_entry.end());
    return true;
  }

  void Mutator::add_dictionary(const ByteArray & entry)
  {
    dictionary_.emplace_back(entry);
  }

  // mutate many --> cross over
  // see https://en.wikipedia.org/wiki/Crossover_(genetic_algorithm)
  // ...

  size_t Mutator::RoundUpToAdd(size_t curr_size, size_t to_add)
  {
    if (curr_size >= max_len_) {
      return 0;
    }
    const size_t remainder = (curr_size + to_add) % size_alignment_;
    if (remainder != 0) {
      to_add = to_add + size_alignment_ - remainder;
    }
    if (curr_size + to_add > max_len_) {
      return max_len_ - curr_size;
    }
    return to_add;
  }

  size_t Mutator::RoundDownToRemove(size_t curr_size, size_t to_remove)
  {
    if (curr_size <= size_alignment_) {
      return 0;
    }
    if (to_remove >= curr_size) {
      return curr_size - size_alignment_;
    }

    size_t result_size = curr_size - to_remove;
    result_size -= (result_size % size_alignment_);
    to_remove = curr_size - result_size;
    if (result_size == 0) {
      to_remove -= size_alignment_;
    }
    if (result_size > max_len_) {
      return curr_size - max_len_;
    }
    return to_remove;
  }

} // namespace trooper
