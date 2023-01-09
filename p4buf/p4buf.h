#ifndef P4BUF_P4BUF_H_
#define P4BUF_P4BUF_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "fmt/core.h"

namespace p4buf {

using bitwidth_t = std::size_t;
using bytewidth_t = std::size_t;

// Polymorphic class representing an arbitrary P4 data type.
class DataTypeSpec {
 public:
  // Number of bits.
  bitwidth_t get_bitwidth() const { return bitwidth_; }

  // Whether this type has members.
  virtual bool has_members() const = 0;

  // View the member name list.
  virtual const std::vector<std::string>& view_members() const = 0;

  // View a readonly member spec.
  virtual std::shared_ptr<const DataTypeSpec> view(std::string name) const = 0;

  // Create a smart pointer to a copy of this object.
  virtual std::unique_ptr<DataTypeSpec> copy_ptr() const = 0;

  // Represent this type as a human-readable string.
  virtual std::string as_string(int indent = 0) const = 0;

  // Print this type spec in a human-readable way.
  void print() const { fmt::print("{}\n", as_string()); };

 protected:
  bitwidth_t bitwidth_ = 0;
};

// Represent the bit<W> type.
class BitTypeSpec : public DataTypeSpec {
 public:
  BitTypeSpec() = default;
  BitTypeSpec(const BitTypeSpec&) = default;
  BitTypeSpec(bitwidth_t bitwidth) { bitwidth_ = bitwidth; };

  // Implement DataTypeSpec.
  bool has_members() const { return false; }
  const std::vector<std::string>& view_members() const {
    return empty_member_names_;
  }
  std::shared_ptr<const DataTypeSpec> view(std::string name) const {
    return nullptr;
  }
  std::unique_ptr<DataTypeSpec> copy_ptr() const {
    return std::make_unique<BitTypeSpec>(*this);
  }
  std::string as_string(int indent = 0) const;

 protected:
  std::vector<std::string> empty_member_names_ = {};
};

// Represent the struct type.
class StructTypeSpec : public DataTypeSpec {
 public:
  StructTypeSpec() = default;
  StructTypeSpec(const StructTypeSpec&);

  // Implement DataTypeSpec.
  bool has_members() const { return true; }
  const std::vector<std::string>& view_members() const { return member_names_; }
  std::shared_ptr<const DataTypeSpec> view(std::string name) const {
    return member_spec_.at(name);
  }
  std::unique_ptr<DataTypeSpec> copy_ptr() const;
  std::string as_string(int indent = 0) const;

  // Add an arbitrary member.
  void add(std::string name, std::unique_ptr<DataTypeSpec> type_spec);

  // Add a bit member.
  void add(std::string name, bitwidth_t bitwidth);

  // Add a struct member.
  void add(std::string name, const StructTypeSpec& type_spec);

 protected:
  std::vector<std::string> member_names_;
  std::map<std::string, std::shared_ptr<DataTypeSpec>> member_spec_;
};

// Memory layouts.
enum class Layout {
  compact,
  byte_aligned,
};

// FieldSpec tells the bit range of a field within a buffer.
struct FieldSpec {
  bytewidth_t byte_offset;
  uint8_t bit_offset;
  bitwidth_t bitwidth;
};

// Schema specifies the memory layout of a buffer accorrding to a type.
class Schema {
 public:
  Schema(std::shared_ptr<const DataTypeSpec> type_spec, Layout layout);

  Layout get_layout() const { return layout_; }
  bytewidth_t get_bytewidth() const { return bytewidth_; }
  const FieldSpec& view(std::string field_name) const {
    return field_spec_.at(field_name);
  }

  // Print this schema in a human-readable way.
  void print() const;

 protected:
  Layout layout_;
  bytewidth_t bytewidth_ = 0;
  std::vector<std::string> field_names_;
  std::map<std::string, FieldSpec> field_spec_;
};

class Buffer {
 public:
  Buffer(std::shared_ptr<const Schema> schema) : schema_(schema) {
    data_ = std::make_unique<std::byte[]>(schema_->get_bytewidth());
  }

  bytewidth_t get_bytewidth() const { return schema_->get_bytewidth(); }

  void set(std::string field_name, uint8_t value);

  void set(FieldSpec spec, uint8_t value, uint8_t value_bitwidth);

  // Print this buffer in a human-readable way.
  void print() const;

 protected:
  std::shared_ptr<const Schema> schema_;
  std::unique_ptr<std::byte[]> data_;
};

}  // namespace p4buf

#endif  // P4BUF_P4BUF_H_
