#include "p4buf/p4buf.h"

namespace p4buf {

std::string BitTypeSpec::as_string(int indent) const {
  return fmt::format("{:{}}bit<{}>", "", indent, bitwidth_);
}

StructTypeSpec::StructTypeSpec(const StructTypeSpec& other) {
  for (const auto& name : other.member_names_) {
    this->add(name, other.member_spec_.at(name)->copy_ptr());
  }
}

std::unique_ptr<DataTypeSpec> StructTypeSpec::copy_ptr() const {
  auto copy = std::make_unique<StructTypeSpec>();
  for (const auto& name : member_names_) {
    copy->add(name, member_spec_.at(name)->copy_ptr());
  }
  return copy;
}

void StructTypeSpec::add(std::string name,
                         std::unique_ptr<DataTypeSpec> type_spec) {
  bitwidth_ += type_spec->get_bitwidth();
  member_names_.push_back(name);
  member_spec_[name] = std::move(type_spec);
}

void StructTypeSpec::add(std::string name, bitwidth_t bitwidth) {
  this->add(name, std::make_unique<BitTypeSpec>(bitwidth));
}

void StructTypeSpec::add(std::string name, const StructTypeSpec& type_spec) {
  this->add(name, type_spec.copy_ptr());
}

std::string StructTypeSpec::as_string(int indent) const {
  std::string s;
  s += fmt::format("{:{}}struct {{\n", "", indent);
  for (const auto& name : member_names_) {
    s += fmt::format("{} {};\n", member_spec_.at(name)->as_string(indent + 4),
                     name);
  }
  s += fmt::format("{:{}}}}", "", indent);
  return s;
}

}  // namespace p4buf
