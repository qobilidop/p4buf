// Editable byte and bit buffers.

#ifndef P4BUF_BUFFER_H_
#define P4BUF_BUFFER_H_

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <optional>

#include "absl/container/flat_hash_map.h"
#include "absl/log/check.h"
#include "absl/strings/string_view.h"

namespace p4buf {

class Buffer;
class BitField;
class BufferEditor;

// Buffer owns a byte array on heap.
class Buffer {
 public:
  // Creates an empty buffer.
  Buffer() = default;

  // Creates a buffer with the given data pointer and size.
  Buffer(std::unique_ptr<std::byte[]> data, std::size_t size)
      : data_(std::move(data)), size_(size) {}

  // Creates a buffer of the given size (in bytes). Optionally sets all bytes to
  // the given initial value.
  Buffer(std::size_t size, std::optional<uint8_t> init_val = std::nullopt)
      : data_(new std::byte[size]), size_(size) {
    if (init_val.has_value()) {
      std::memset(data_.get(), init_val.value(), this->size());
    }
  }

  // Creates a buffer with data copied from the given bytes.
  Buffer(std::initializer_list<uint8_t> bytes)
      : data_(new std::byte[bytes.size()]), size_(bytes.size()) {
    std::memcpy(data_.get(), std::data(bytes), this->size());
  }

  // Creates a buffer with data copied from the other one.
  Buffer(const Buffer& other) : data_(new std::byte[other.size()]) {
    std::memcpy(data_.get(), other.data(), other.size());
  }

  // Creates a buffer with data moved from the other one.
  Buffer(Buffer&& other) = default;

  // Copies data from the other buffer to this one.
  Buffer& operator=(const Buffer& other) {
    *this = Buffer(other);
    return *this;
  }

  // Moves data from the other buffer to this one.
  Buffer& operator=(Buffer&& other) = default;

  ~Buffer() = default;

  // Returns a reference to the byte at specified index, with bounds checking.
  std::byte& operator[](const std::size_t index) {
    CHECK(index < size_);
    return data_[index];
  }

  // Returns a const reference to the byte at specified index, with bounds
  // checking.
  const std::byte& at(const std::size_t index) const {
    CHECK(index < size_);
    return data_[index];
  }

  // Returns a pointer to the beginning of the data.
  std::byte* data() const { return data_.get(); }

  // Returns the size (in bytes) of the data.
  std::size_t size() const { return size_; }

 private:
  std::unique_ptr<std::byte[]> data_ = nullptr;
  std::size_t size_ = 0;
};

// Bit field references a contiguous range of bits in a buffer.
class BitField {
 public:
  // Creates an empty bit field.
  BitField() = default;

  // Creates a bit field with given offset and width (in bits) from a buffer,
  // with bounds checking.
  BitField(std::shared_ptr<Buffer> buffer, std::size_t offset,
           std::size_t width)
      : buffer_(buffer), offset_(offset), width_(width) {
    CHECK(offset_ + width_ <= buffer_->size() * 8);
  }

  // Creates a bit field representing the whole buffer.
  BitField(std::shared_ptr<Buffer> buffer)
      : buffer_(buffer), offset_(0), width_(buffer->size() * 8) {}

  // Creates a bit field representing the given bytes.
  BitField(std::initializer_list<uint8_t> bytes)
      : BitField(std::make_shared<Buffer>(bytes)) {}

  // Creates a bit field representing the given byte.
  BitField(uint8_t byte) : BitField({byte}) {}

  // Creates a bit field representing the given bytes.
  BitField(uint16_t bytes);

  // Creates a bit field representing the given bytes.
  BitField(uint32_t bytes);

  // Creates a bit field representing the given bytes.
  BitField(uint64_t bytes);

  // Disallow copy constructor.
  BitField(const BitField&) = delete;

  // Disallow move constructor.
  BitField(BitField&&) = default;

  // Copies the bits from the other bit field into this one, in a right-alienged
  // way. For example:
  //
  //   00000 = 101      => 00101
  //   00000 = 10100101 => 00101
  BitField& operator=(const BitField& other);

  ~BitField() = default;

  // Returns a pointer to the underlying buffer.
  std::shared_ptr<Buffer> buffer() const { return buffer_; }

  // Returns the offset (in bits).
  std::size_t offset() const { return offset_; }

  // Returns the width (in bits).
  std::size_t width() const { return width_; }

 private:
  std::shared_ptr<Buffer> buffer_ = nullptr;
  std::size_t offset_ = 0;
  std::size_t width_ = 0;
};

// Buffer editor facilitates editing a buffer with named bit
// fields.
class BufferEditor {
 public:
  using FieldSpec =
      std::tuple<std::size_t /* offset */, std::size_t /* width */>;

  BufferEditor() = default;

  BitField operator[](absl::string_view name) {
    auto [offset, width] = field_spec.at(name);
    return BitField(buffer, offset, width);
  }

  absl::flat_hash_map<std::string, FieldSpec> field_spec = {};
  std::shared_ptr<Buffer> buffer = nullptr;
};

}  // namespace p4buf

#endif  // P4BUF_BUFFER_H_
