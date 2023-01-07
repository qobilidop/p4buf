#include "type_spec.h"

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

}  // namespace p4buf
