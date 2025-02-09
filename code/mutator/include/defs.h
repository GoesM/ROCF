#ifndef THIRD_PARTY_DEFS_H_
#define THIRD_PARTY_DEFS_H_

#include <cstddef>
#include <cstdint>
#include <random>
#include <string_view>
#include <vector>
#include <span>

namespace trooper {

  // Mersenne Twister PseudoRandom Generator, 64bits
  using Rng = std::mt19937_64;

  using ByteArray = std::vector < uint8_t >;
  using ByteSpan = std::span < const uint8_t >;

  inline ByteSpan AsByteSpan(std::string_view str)
  {
    return ByteSpan(reinterpret_cast < const uint8_t * > (str.data()), str.size());
  }

  inline std::string_view AsStringView(ByteSpan str)
  {
    return std::string_view(reinterpret_cast < const char * > (str.data()), str.size());
  }

  // KPathMax >= PATH_MAX
  constexpr size_t kPathMax = 4096;

}  // namespace trooper

#endif  // THIRD_PARTY_DEFS_H_
