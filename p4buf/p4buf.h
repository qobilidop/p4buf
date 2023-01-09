#ifndef P4BUF_P4BUF_H_
#define P4BUF_P4BUF_H_

#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace p4buf {

using bitwidth_t = std::size_t;
using bytewidth_t = std::size_t;

// Polymorphic class representing an arbitrary data type.
class DataTypeSpec {
 public:
  DataTypeSpec() = default;
  DataTypeSpec(bitwidth_t bitwidth) : bitwidth_(bitwidth) {}

  // Number of bits.
  bitwidth_t get_bitwidth() const { return bitwidth_; }

  // Whether this type has members.
  virtual bool has_members() const = 0;

  // Traverse all members and do something.
  // Member specs are readonly to the visitor function.
  using VisitorFunc = std::function<void(std::string_view,
                                         std::shared_ptr<const DataTypeSpec>)>;
  virtual void traverse(VisitorFunc visit) const = 0;

  // View a readonly member spec.
  virtual std::shared_ptr<const DataTypeSpec> view(std::string name) const = 0;

  // Copy this object and return a smart pointer.
  virtual std::unique_ptr<DataTypeSpec> copy_ptr() const = 0;

 protected:
  bitwidth_t bitwidth_ = 0;
};

class BitTypeSpec : public DataTypeSpec {
 public:
  BitTypeSpec() = default;
  BitTypeSpec(const BitTypeSpec&) = default;
  BitTypeSpec(bitwidth_t bitwidth) : DataTypeSpec(bitwidth) {}

  // Implement DataTypeSpec.
  bool has_members() const { return false; }
  void traverse(VisitorFunc visit) const {}
  std::shared_ptr<const DataTypeSpec> view(std::string name) const {
    return nullptr;
  }
  std::unique_ptr<DataTypeSpec> copy_ptr() const {
    return std::make_unique<BitTypeSpec>(*this);
  }
};

class StructTypeSpec : public DataTypeSpec {
 public:
  StructTypeSpec() = default;
  StructTypeSpec(const StructTypeSpec&) = default;
  StructTypeSpec(std::initializer_list<std::tuple<std::string, bitwidth_t>>);

  // Implement DataTypeSpec.
  bool has_members() const { return true; }
  void traverse(VisitorFunc visit) const {
    for (const auto& name : member_names_) {
      visit(name, member_spec_.at(name));
    }
  }
  std::shared_ptr<const DataTypeSpec> view(std::string name) const {
    return member_spec_.at(name);
  }
  std::unique_ptr<DataTypeSpec> copy_ptr() const {
    auto copy = std::make_unique<StructTypeSpec>();
    for (const auto& name : member_names_) {
      copy->add(name, member_spec_.at(name)->copy_ptr());
    }
    return copy;
  }

  // Add an arbitrary member.
  void add(std::string name, std::unique_ptr<DataTypeSpec> type_spec);

  // Add a bit member.
  void add_bit(std::string name, bitwidth_t bitwidth);

  // Add a struct member.
  void add_struct(std::string name, const StructTypeSpec& type_spec);

 protected:
  std::vector<std::string> member_names_;
  std::map<std::string, std::shared_ptr<DataTypeSpec>> member_spec_;
};

}  // namespace p4buf

#endif  // P4BUF_P4BUF_H_
