// Common utilities for bit-level operations.

#ifndef P4BUF_BIT_H_
#define P4BUF_BIT_H_

#include <cstddef>

namespace p4buf {

// Copies count bits from the byte array pointed to by src (with src_offset in
// bits) to the byte array pointed to by dest (with dest_offset in bits).
//
// All undefined behaviors in std::memcpy are also assumed here.
void BitMemCpy(std::byte* dest, const std::byte* src, std::size_t dest_offset,
               std::size_t src_offset, std::size_t count);

}  // namespace p4buf

#endif  // P4BUF_BIT_H_
