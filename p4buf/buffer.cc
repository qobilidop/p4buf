#include "p4buf/buffer.h"

#include <bitset>
#include <iostream>

#include "absl/base/internal/endian.h"
#include "p4buf/bit.h"

namespace p4buf {

BitField::BitField(uint16_t bytes)
    : buffer_(std::make_shared<Buffer>(2)), offset_(0), width_(16) {
  bytes = absl::ghtons(bytes);
  std::memcpy(buffer_->data(), &bytes, buffer_->size());
}

BitField::BitField(uint32_t bytes)
    : buffer_(std::make_shared<Buffer>(4)), offset_(0), width_(32) {
  bytes = absl::ghtonl(bytes);
  std::memcpy(buffer_->data(), &bytes, buffer_->size());
}

BitField::BitField(uint64_t bytes)
    : buffer_(std::make_shared<Buffer>(8)), offset_(0), width_(64) {
  bytes = absl::ghtonll(bytes);
  std::memcpy(buffer_->data(), &bytes, buffer_->size());
}

BitField& BitField::operator=(const BitField& other) {
  // TODO: Properly handle the case when 2 buffers are the same one.

  // Guard empty field or value.
  if (width_ == 0 || other.width() == 0) {
    return *this;
  }

  // Right align the bit fields.
  std::size_t width = std::min(width_, other.width());
  std::size_t this_offset = offset_ + width_ - width;
  std::size_t other_offset = other.offset() + other.width() - width;

  BitMemCpy(buffer_->data(), other.buffer()->data(), this_offset, other_offset,
            width);

  return *this;
};

}  // namespace p4buf
