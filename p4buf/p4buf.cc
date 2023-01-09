#include "p4buf/p4buf.h"

#include <string>

namespace p4buf {

StructTypeSpec::StructTypeSpec(
    std::initializer_list<std::tuple<std::string, bitwidth_t>> l) {
  for (auto [name, bitwidth] : l) {
    this->add_bit(name, bitwidth);
  }
}

void StructTypeSpec::add(std::string name,
                         std::unique_ptr<DataTypeSpec> type_spec) {
  bitwidth_ += type_spec->get_bitwidth();
  member_names_.push_back(name);
  member_spec_[name] = std::move(type_spec);
}

void StructTypeSpec::add_bit(std::string name, bitwidth_t bitwidth) {
  this->add(name, std::make_unique<BitTypeSpec>(bitwidth));
}

void StructTypeSpec::add_struct(std::string name,
                                const StructTypeSpec& type_spec) {
  this->add(name, type_spec.copy_ptr());
}

class RawBuffer {
 public:
  RawBuffer(const std::byte *data, bytewidth_t bytewidth)
      : data_(data), bytewidth_(bytewidth) {}

  RawBuffer(bytewidth_t byte_size) : bytewidth_(byte_size) {
    data_ = new std::byte[byte_size];
  }

 protected:
  const std::byte *data_;
  uint8_t bytewidth_;
};

}  // namespace p4buf
