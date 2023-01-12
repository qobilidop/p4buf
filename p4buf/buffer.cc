#include "p4buf/buffer.h"

#include <bitset>
#include <iostream>

namespace p4buf {

void Buffer::print() {
  std::cout << "bytes:";
  for (std::size_t i = 0; i < size_; ++i) {
    std::cout << " " << std::bitset<8>(std::to_integer<int>(data_[i]));
  }
  std::cout << "\n";
}

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

BitField::BitField(std::initializer_list<uint8_t> bytes,
                   std::optional<std::size_t> width)
    : buffer_(std::make_shared<Buffer>(bytes)),
      offset_(0),
      width_(bytes.size() * 8) {
  if (width.has_value() && width.value() < width_) {
    offset_ = width_ - width.value();
    width_ = width.value();
  }
}

// TODO: Handle the more complicated case where value shares the same underlying
// Buffer.
void BitField::Write(const BitField& value) {
  // Guard against empty field or value.
  if (width_ == 0 || value.width() == 0) {
    return;
  }

  std::size_t value_offset = value.offset();
  std::size_t field_offset = offset_;
  std::size_t field_width = std::min(width_, value.width());

  // Read and write bytes one by one.
  while (field_width >= 8) {
    auto byte = internal::ReadOneByte(value.buffer(), value_offset);
    internal::WriteOneByte(buffer_, field_offset, byte);
    value_offset += 8;
    field_offset += 8;
    field_width -= 8;
  }

  // Read and write the remaining few bits.
  if (field_width > 0) {
    // It's okay to read a whole byte. We just need to limit bits in write.
    auto byte = internal::ReadOneByte(value.buffer(), value_offset);
    internal::WriteSomeBits(buffer_, field_offset, byte, field_width);
  }
};

BufferEditor::BufferEditor(const DataTypeSpec* type_spec) {
  std::string prefix = "";
  std::size_t offset = 0;

  std::function<void(std::string, const DataTypeSpec*)> visit;
  visit = [this, &prefix, &offset, &visit](
              std::string name, const DataTypeSpec* type_spec) -> void {
    if (!type_spec->has_members()) {
      // This is a simple bit field.
      std::string field_name = prefix + name;
      this->AddField(field_name, offset, type_spec->bitwidth());
      offset += type_spec->bitwidth();
    } else {
      // This is a struct. Recurse into its members.
      std::string prefix_backup = prefix;
      prefix += (name + "/");
      for (const auto& name : type_spec->view_members()) {
        visit(name, type_spec->view(name).get());
      }
      prefix = prefix_backup;
    }
  };

  visit("", type_spec);
};

}  // namespace p4buf
