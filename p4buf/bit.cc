#include "p4buf/bit.h"

namespace p4buf {
namespace {

const std::byte kByteMask{0xff};

// Reads one byte from the buffer, starting from the bit_offset.
inline std::byte ReadOneByte(const std::byte* const buffer,
                             std::size_t bit_offset) {
  auto byte_offset = bit_offset / 8;
  bit_offset %= 8;

  if (bit_offset == 0) {
    // Aligned case.
    return buffer[byte_offset];
  } else {
    // Unaligned case.
    // Read the first (8 - bit_offset) bits.
    auto byte = (buffer[byte_offset] << bit_offset);
    // Read the remaining (bit_offset) bits.
    byte |= (buffer[byte_offset + 1] >> (8 - bit_offset));
    return byte;
  }
}

// Writes one byte to the buffer, starting from the bit_offset.
inline void WriteOneByte(std::byte* const buffer, std::size_t bit_offset,
                         std::byte byte) {
  std::size_t byte_offset = bit_offset / 8;
  bit_offset %= 8;

  if (bit_offset == 0) {
    // Aligned case.
    buffer[byte_offset] = byte;
  } else {
    // Unaligned case.
    // Write the first (8 - bit_offset) bits.
    buffer[byte_offset] &= ~(kByteMask >> bit_offset);
    buffer[byte_offset] |= (byte >> bit_offset);
    // Write the remaining (bit_offset) bits.
    buffer[byte_offset + 1] &= ~(kByteMask << (8 - bit_offset));
    buffer[byte_offset + 1] |= (byte << (8 - bit_offset));
  }
}

// Writes limited bits to the buffer, starting from the bit_offset. Won't write
// anything beyond the buffer boundary and bit_width_limit.
inline void WriteSomeBits(std::byte* const buffer, std::size_t bit_offset,
                          std::byte byte, std::size_t bit_width_limit = 8) {
  std::size_t byte_offset = bit_offset / 8;
  bit_offset %= 8;

  std::byte mask{kByteMask};
  if (bit_width_limit < 8) {
    // Only keep the first (bit_width_limit) bits.
    mask <<= (8 - bit_width_limit);
    byte &= mask;
  }

  if (bit_offset == 0) {
    // Aligned case.
    // Write the first bit_width_limit bits.
    buffer[byte_offset] &= ~mask;
    buffer[byte_offset] |= byte;
  } else {
    // Unaligned case.
    // Write the first (8 - bit_offset) bits.
    buffer[byte_offset] &= ~(mask >> bit_offset);
    buffer[byte_offset] |= (byte >> bit_offset);
    if (bit_width_limit > 8 - bit_offset) {
      // If here are more to write, write the remaining
      // (bit_width_limit - (8 - bit_offset)) bits.
      buffer[byte_offset + 1] &= ~(mask << (8 - bit_offset));
      buffer[byte_offset + 1] |= (byte << (8 - bit_offset));
    }
  }
}

}  // namespace

void BitMemCpy(std::byte* dest, const std::byte* src, std::size_t dest_offset,
               std::size_t src_offset, std::size_t count) {
  // Read and write bytes one by one.
  for (; count >= 8; src_offset += 8, dest_offset += 8, count -= 8) {
    auto byte = ReadOneByte(src, src_offset);
    WriteOneByte(dest, dest_offset, byte);
  }

  // Read and write the remaining few bits.
  if (count > 0) {
    // It's okay to read a whole byte. We just need to limit bits in write.
    auto byte = ReadOneByte(src, src_offset);
    WriteSomeBits(dest, dest_offset, byte, count);
  }
}

}  // namespace p4buf
