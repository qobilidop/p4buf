// P4 data type specs.
//
// We try to follow:
// https://github.com/p4lang/p4runtime/blob/main/proto/p4/config/v1/p4types.proto.

#ifndef P4BUF_TYPE_SPEC_H_
#define P4BUF_TYPE_SPEC_H_

#include <string>
#include <variant>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "p4buf/utility.h"

namespace p4buf {

class BitTS;
class StructTS;

// General P4 data type spec.
using DataTS = std::variant<Box<BitTS>, Box<StructTS>>;

// Generates a bit struct from the given P4 data type spec.
void GenerateBitStruct(const DataTS& spec);

// P4 bit type spec.
class BitTS {
 public:
  BitTS() = default;
  BitTS(const BitTS&) = default;
  BitTS(BitTS&&) = default;
  BitTS(size_t width) : width_(width) {}

  // Width in bits.
  std::size_t width() const { return width_; }

 protected:
  std::size_t width_ = 0;
};

// P4 struct type spec.
class StructTS {
 public:
  StructTS() = default;
  StructTS(const StructTS&) = default;
  StructTS(StructTS&&) = default;
  StructTS(std::initializer_list<std::tuple<std::string, DataTS>> specs);

  // Gets a member type spec.
  template <class T>
  T& Get(absl::string_view name) {
    return *std::get<Box<T>>(member_spec_[name]);
  }

  // Width in bits.
  size_t width() const { return width_; }

 protected:
  std::size_t width_ = 0;
  std::vector<std::string> member_names_;
  absl::flat_hash_map<std::string, DataTS> member_spec_;
};

}  // namespace p4buf

#endif  // P4BUF_TYPE_SPEC_H_
