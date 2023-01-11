// Editable byte and bit buffers.

#ifndef P4BUF_BUFFER_H_
#define P4BUF_BUFFER_H_

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <optional>

namespace p4buf {

class Buffer;
class BufferView;

// Buffer is a raw byte buffer without extra structures.
class Buffer {
 public:
  // Creates a Buffer of the given size (in bytes), and an optional initial
  // value for each byte.
  Buffer(std::size_t size, std::optional<uint8_t> init_val = std::nullopt)
      : data_(new std::byte[size]), size_(size) {
    if (init_val.has_value()) {
      std::memset(this->data(), init_val.value(), this->size());
    }
  }

  // Creates a Buffer from the given bytes.
  Buffer(std::initializer_list<uint8_t> bytes)
      : data_(new std::byte[bytes.size()]), size_(bytes.size()) {
    std::memcpy(this->data(), std::data(bytes), this->size());
  }

  // Returns a pointer to the beginning of the Buffer.
  std::byte* data() const { return data_.get(); }

  // Returns the size (in bytes) of the Buffer.
  std::size_t size() const { return size_; }

 private:
  std::unique_ptr<std::byte[]> data_;
  std::size_t size_;
};

// BufferView is a bit range view of a Buffer.
//
// BufferView can be used to edit the underlying Buffer.
class BufferView {
 public:
  // Creates a BufferView from a Buffer.
  BufferView(std::shared_ptr<Buffer> buffer, std::size_t bit_offset,
             std::size_t bit_width)
      : buffer_(buffer), bit_offset_(bit_offset), bit_width_(bit_width) {}

  // Creates a BufferView from a new Buffer with given bytes.
  //
  // An optional bit_width (n) could be provided to select the right-most n
  // bits. For example, to express 9 bits of 1s, one could write:
  //
  //   auto nine_ones = BufferView({0b1, 0xff}, 9);
  BufferView(std::initializer_list<uint8_t> bytes,
             std::optional<std::size_t> bit_width = std::nullopt)
      : buffer_(std::make_shared<Buffer>(bytes)),
        bit_offset_(0),
        bit_width_(bytes.size() * 8) {
    if (bit_width.has_value() && bit_width.value() < bit_width_) {
      bit_offset_ = bit_width_ - bit_width.value();
      bit_width_ = bit_width.value();
    }
  }

  // Writes the value (represented by another BufferView) to this BufferView.
  //
  // To understand how the write is done, imagine the two BufferViews are
  // left-aligned, then write the overlapping range of the value into this
  // Buffer.
  //
  // For example, using a binary representation and some pseudocode:
  //
  //   00000.Write(111) == 11100
  //   00000.Write(1010010) == 10100
  void Write(const BufferView& value);

  // Returns a pointer to the underlying Buffer.
  std::shared_ptr<Buffer> buffer() const { return buffer_; }

  // Returns the starting bit offset of the range.
  std::size_t bit_offset() const { return bit_offset_; }

  // Returns the total number of bits in the range.
  std::size_t bit_width() const { return bit_width_; }

 private:
  std::shared_ptr<Buffer> buffer_;
  std::size_t bit_offset_;
  std::size_t bit_width_;
};

}  // namespace p4buf

#endif  // P4BUF_BUFFER_H_
