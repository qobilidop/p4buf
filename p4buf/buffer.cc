#include "p4buf/buffer.h"

namespace p4buf {
namespace internal {

const std::byte kByteMask{0xff};

// Reads one byte from the buffer, starting from the bit_offset, assuming the
// buffer is padded with infinite 0s.
inline std::byte ReadOneByte(std::shared_ptr<Buffer> buffer,
                             std::size_t bit_offset) {
  std::size_t byte_offset = bit_offset / 8;
  if (byte_offset >= buffer->size()) {
    // Out of range. Just return 0.
    return std::byte{0};
  }
  bit_offset %= 8;

  if (bit_offset == 0) {
    // Aligned case.
    return buffer->data()[byte_offset];
  } else {
    // Unaligned case.
    // Read the first (8 - bit_offset) bits.
    std::byte byte = (buffer->data()[byte_offset] << bit_offset);
    if (byte_offset + 1 < buffer->size()) {
      // If within range, read the remaining (bit_offset) bits.
      byte |= (buffer->data()[byte_offset + 1] >> (8 - bit_offset));
    }
    return byte;
  }
}

// Writes one byte to the buffer, starting from the bit_offset. Won't write
// anything beyond the buffer boundary.
inline void WriteOneByte(std::shared_ptr<Buffer> buffer, std::size_t bit_offset,
                         std::byte byte) {
  std::size_t byte_offset = bit_offset / 8;
  if (byte_offset >= buffer->size()) {
    // Out of range. Don't write anything.
    return;
  }
  bit_offset %= 8;

  if (bit_offset == 0) {
    // Aligned case.
    buffer->data()[byte_offset] = byte;
  } else {
    // Unaligned case.
    // Write the first (8 - bit_offset) bits.
    buffer->data()[byte_offset] &= ~(kByteMask >> bit_offset);
    buffer->data()[byte_offset] |= (byte >> bit_offset);
    if (byte_offset + 1 < buffer->size()) {
      // If within range, write the remaining (bit_offset) bits.
      buffer->data()[byte_offset + 1] &= ~(kByteMask << (8 - bit_offset));
      buffer->data()[byte_offset + 1] |= (byte << (8 - bit_offset));
    }
  }
}

// Writes limited bits to the buffer, starting from the bit_offset. Won't write
// anything beyond the buffer boundary and bit_width_limit.
inline void WriteSomeBits(std::shared_ptr<Buffer> buffer,
                          std::size_t bit_offset, std::byte byte,
                          std::size_t bit_width_limit = 8) {
  std::size_t byte_offset = bit_offset / 8;
  if (byte_offset >= buffer->size()) {
    // Out of range. Don't write anything.
    return;
  }
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
    buffer->data()[byte_offset] &= ~mask;
    buffer->data()[byte_offset] |= byte;
  } else {
    // Unaligned case.
    // Write the first (8 - bit_offset) bits.
    buffer->data()[byte_offset] &= ~(mask >> bit_offset);
    buffer->data()[byte_offset] |= (byte >> bit_offset);
    if (byte_offset + 1 < buffer->size() && bit_width_limit > 8 - bit_offset) {
      // If within range and there are more to write, write the remaining
      // (bit_width_limit - (8 - bit_offset)) bits.
      buffer->data()[byte_offset + 1] &= ~(mask << (8 - bit_offset));
      buffer->data()[byte_offset + 1] |= (byte << (8 - bit_offset));
    }
  }
}

}  // namespace internal

void BufferView::Write(const BufferView& value) {
  std::size_t value_bit_offset = value.bit_offset();
  std::size_t bit_offset = bit_offset_;
  std::size_t bit_width = std::min(bit_width_, value.bit_width());

  // Read and write bytes one by one.
  while (bit_width >= 8) {
    auto byte = internal::ReadOneByte(value.buffer(), value_bit_offset);
    internal::WriteOneByte(buffer_, bit_offset, byte);
    value_bit_offset += 8;
    bit_offset += 8;
    bit_width -= 8;
  }

  // Read and write the remaining few bits.
  if (bit_width > 0) {
    // It's okay to read a whole byte. We just need to limit bits in write.
    auto byte = internal::ReadOneByte(value.buffer(), value_bit_offset);
    internal::WriteSomeBits(buffer_, bit_offset, byte, bit_width);
  }
};

}  // namespace p4buf
