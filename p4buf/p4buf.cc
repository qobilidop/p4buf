#include "p4buf/p4buf.h"

#include <functional>

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

Schema::Schema(std::shared_ptr<const DataTypeSpec> type_spec, Layout layout)
    : layout_(layout) {
  std::string prefix = "";
  FieldSpec field_spec = {
      .byte_offset = 0,
      .bit_offset = 0,
  };

  std::function<void(std::string, std::shared_ptr<const DataTypeSpec>)> visit;
  visit = [this, &layout, &prefix, &field_spec, &visit](
              std::string name,
              std::shared_ptr<const DataTypeSpec> type_spec) -> void {
    if (!type_spec->has_members()) {
      // This is a simple bit field.
      std::string field_name = prefix + name;
      field_names_.push_back(prefix + name);
      field_spec.bitwidth = type_spec->get_bitwidth();
      field_spec_[field_name] = field_spec;
      switch (layout) {
        case Layout::compact:

          field_spec.byte_offset +=
              (field_spec.bit_offset + field_spec.bitwidth) / 8;
          field_spec.bit_offset =
              (field_spec.bit_offset + field_spec.bitwidth) % 8;
          break;
        case Layout::byte_aligned:
          field_spec.byte_offset +=
              (field_spec.bit_offset + field_spec.bitwidth + 7) / 8;
          break;
      }
    } else {
      // This is a struct. Recurse into its members.
      std::string prefix_backup = prefix;
      prefix += (name + "/");
      for (const auto& name : type_spec->view_members()) {
        visit(name, type_spec->view(name));
      }
      prefix = prefix_backup;
    }
  };

  visit("", type_spec);
  bytewidth_ = field_spec.bit_offset == 0 ? field_spec.byte_offset
                                          : field_spec.byte_offset + 1;
}

void Schema::print() const {
  switch (layout_) {
    case Layout::compact:
      fmt::print("layout: compact\n");
      break;
    case Layout::byte_aligned:
      fmt::print("layout: byte aligned\n");
      break;
  }
  fmt::print("bytewidth: {}\n", bytewidth_);
  fmt::print("fields:\n");
  for (const auto& name : field_names_) {
    auto field_spec = field_spec_.at(name);
    fmt::print("    {}: {{ byte_offset: {}, bit_offset: {}, bitwidth: {} }}\n",
               name, field_spec.byte_offset, field_spec.bit_offset,
               field_spec.bitwidth);
  }
}

void Buffer::set(std::string field_name, uint8_t value) {
  set(schema_->view(field_name), value, 8);
}

void Buffer::set(FieldSpec spec, uint8_t value, uint8_t value_bitwidth) {
  if (spec.bitwidth > value_bitwidth) {
    auto offset = spec.bit_offset + spec.bitwidth - value_bitwidth;
    spec.byte_offset += (offset / 8);
    spec.bit_offset = offset % 8;
    spec.bitwidth = value_bitwidth;
    set(spec, value, value_bitwidth);
  } else if (value_bitwidth > 0) {
    std::byte mask{0}, temp{value};
    mask = std::byte(((1 << spec.bitwidth) - 1)) << (8 - spec.bitwidth);
    temp <<= (8 - spec.bitwidth);
    mask >>= spec.bit_offset;
    temp >>= spec.bit_offset;
    data_[spec.byte_offset] &= ~mask;
    data_[spec.byte_offset] |= temp;
    if (spec.bit_offset + spec.bitwidth > 8) {
      spec.byte_offset += 1;
      spec.bitwidth = spec.bit_offset + spec.bitwidth - 8;
      spec.bit_offset = 0;
      value_bitwidth -= spec.bitwidth;
      set(spec, value, value_bitwidth);
    }
  }
}

void Buffer::print() const {
  bytewidth_t bytewidth = get_bytewidth();
  for (bytewidth_t i = 0; i < bytewidth; i++) {
    fmt::print("{:08b} ", data_[i]);
  }
  fmt::print("\n");
}

}  // namespace p4buf
