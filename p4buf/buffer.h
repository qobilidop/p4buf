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
#include "absl/strings/string_view.h"
#include "p4buf/type_spec.h"

namespace p4buf {

class Buffer;
class BitField;
class BufferEditor;

// Buffer is a continuous array of bytes.
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

  // Prints out bytes in a human-readable format.
  void print();

  // Returns a pointer to the beginning of the Buffer.
  std::byte* data() const { return data_.get(); }

  // Returns the size (in bytes) of the Buffer.
  std::size_t size() const { return size_; }

 private:
  std::unique_ptr<std::byte[]> data_;
  std::size_t size_;
};

// BitField is an editable range of bits in a Buffer.
class BitField {
 public:
  // Creates a BitField from a Buffer with given offset (in bits) and width (in
  // bits).
  BitField(std::shared_ptr<Buffer> buffer, std::size_t offset,
           std::size_t width)
      : buffer_(buffer), offset_(offset), width_(width) {}

  // Creates a BitField representing the given value.
  //
  // An optional width can be given to further select the last n bits. For
  // example, to express 11 bits of 1s, one could write:
  //
  //   BitField({0b111, 0xff}, 11)
  //
  // If the given with is larger than the bit width of the given bytes, it will
  // be ignored.
  BitField(std::initializer_list<uint8_t> bytes,
           std::optional<std::size_t> width = std::nullopt);

  // Writes the value into this BitField.
  //
  // The value is represented by another BitField. To write, the two BitFields
  // are first left-aligned, and then the overlapping bit range of the value is
  // written into this BitField.
  //
  // For example, in binary encoding and some pseudocode, to write a 5-bit
  // BitField:
  //
  //   00000.Write(1010) == 10100
  //   00000.Write(101010) == 10101
  void Write(const BitField& value);

  // Returns a pointer to the underlying Buffer.
  std::shared_ptr<Buffer> buffer() const { return buffer_; }

  // Returns the offset (in bits) of this BitField.
  std::size_t offset() const { return offset_; }

  // Returns the width (in bits) of this BitField.
  std::size_t width() const { return width_; }

 private:
  std::shared_ptr<Buffer> buffer_;
  std::size_t offset_;
  std::size_t width_;
};

// BufferEditor facilitates editing named BitFields of a Buffer.
class BufferEditor {
 public:
  // Generates an empty BufferEditor.
  BufferEditor() = default;

  // Generates a BufferEditor from a DataTypeSpec.
  BufferEditor(const DataTypeSpec* type_spec);

  // Sets to edit the given buffer.
  void Edit(std::shared_ptr<Buffer> buffer) { buffer_ = buffer; }

  // Adds a named BitField. Could overwrite a previous one with the same name.
  void AddField(absl::string_view name, std::size_t offset, std::size_t width) {
    field_spec_[name] = {offset, width};
  }

  BitField GetField(absl::string_view name) {
    auto [offset, width] = field_spec_.at(name);
    return BitField(buffer_, offset, width);
  };

  std::shared_ptr<Buffer> buffer() { return buffer_; }

 private:
  std::shared_ptr<Buffer> buffer_;
  absl::flat_hash_map<std::string, std::tuple<std::size_t, std::size_t>>
      field_spec_;
};

}  // namespace p4buf

#endif  // P4BUF_BUFFER_H_
