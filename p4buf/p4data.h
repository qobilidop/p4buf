// P4 data representation.
//
// Reference:
// https://github.com/p4lang/p4runtime/blob/main/proto/p4/v1/p4data.proto
// https://github.com/p4lang/p4runtime/blob/main/proto/p4/config/v1/p4types.proto.

#ifndef P4BUF_P4DATA_H_
#define P4BUF_P4DATA_H_

#include <string>
#include <variant>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "p4buf/buffer.h"
#include "p4buf/utility.h"

namespace p4buf {

// Core abstractions.
class P4Type;
class P4Data;

// Supported P4 types.
class P4BitT;
class P4StructT;
class P4TupleT;
using P4TypeVariant = std::variant<P4BitT, Box<P4StructT>, Box<P4TupleT>>;

// P4 bit type.
class P4BitT {
 public:
  P4BitT() = default;
  P4BitT(const P4BitT&) = default;
  P4BitT(P4BitT&&) = default;
  P4BitT(size_t bitwidth) : bitwidth_(bitwidth) {}

  std::size_t bitwidth() const { return bitwidth_; }

 private:
  std::size_t bitwidth_ = 0;
};

// P4 struct type.
class P4StructT {
 public:
  using Visitor = std::function<void(absl::string_view, const P4Type&)>;

  P4StructT() = default;
  P4StructT(const P4StructT&) = default;
  P4StructT(P4StructT&&) = default;
  P4StructT(
      std::initializer_list<std::tuple<std::string, P4TypeVariant>> members);

  // Accesses member type.
  const P4Type& operator[](absl::string_view name) const {
    return members_.at(name);
  }

  void Traverse(Visitor visit) const;

  std::size_t bitwidth() const { return bitwidth_; }

 private:
  std::size_t bitwidth_ = 0;
  std::vector<std::string> member_names_;
  absl::flat_hash_map<std::string, P4Type> members_;
};

// P4 tuple type.
class P4TupleT {
 public:
  using Visitor = std::function<void(std::size_t, const P4Type&)>;

  P4TupleT() = default;
  P4TupleT(const P4TupleT&) = default;
  P4TupleT(P4TupleT&&) = default;
  P4TupleT(std::initializer_list<P4TypeVariant> members);

  // Accesses member type.
  const P4Type& operator[](std::size_t index) const {
    return members_.at(index);
  }

  void Traverse(Visitor visit) const;

  std::size_t bitwidth() const { return bitwidth_; }

 private:
  std::size_t bitwidth_ = 0;
  std::vector<P4Type> members_;
};

// A helper class to represent any supported P4 type.
class P4Type {
 public:
  P4Type(const P4TypeVariant& variant) : variant_(variant) {}
  P4Type(P4TypeVariant&& variant) : variant_(std::move(variant)) {}

  // Accesses member type assuming this is a struct.
  const P4Type& operator[](absl::string_view name) const;

  // Accesses member type assuming this is a tuple.
  const P4Type& operator[](std::size_t index) const;

  std::size_t bitwidth() const;

  const P4TypeVariant& variant() const { return variant_; }

 private:
  P4TypeVariant variant_;
};

class P4Data {
 public:
  using FieldSpec =
      std::tuple<std::size_t /* offset */, std::size_t /* width */>;

  P4Data(const P4Type& type, std::optional<uint8_t> init_val = std::nullopt);

  BitField operator[](absl::string_view path);

  void NewBuffer(std::optional<uint8_t> init_val = std::nullopt);

  const P4Type& type() const { return type_; }

  std::shared_ptr<Buffer> buffer() const { return buffer_; }

 private:
  const P4Type type_;
  absl::flat_hash_map<std::string, FieldSpec> field_spec_;
  std::shared_ptr<Buffer> buffer_ = nullptr;
};

}  // namespace p4buf

#endif  // P4BUF_P4DATA_H_
