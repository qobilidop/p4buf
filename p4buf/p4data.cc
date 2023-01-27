#include "p4buf/p4data.h"

namespace p4buf {

P4StructT::P4StructT(
    std::initializer_list<std::tuple<std::string, P4TypeVariant>> members) {
  for (auto& [name, p4type_var] : members) {
    P4Type p4type(std::move(p4type_var));
    bitwidth_ += p4type.bitwidth();
    member_names_.emplace_back(name);
    members_.emplace(name, std::move(p4type));
  }
}

void P4StructT::Traverse(Visitor visit) const {
  for (const auto& name : member_names_) {
    visit(name, members_.at(name));
  }
}

P4TupleT::P4TupleT(std::initializer_list<P4TypeVariant> members) {
  for (auto& p4type_var : members) {
    P4Type p4type(std::move(p4type_var));
    bitwidth_ += p4type.bitwidth();
    members_.emplace_back(std::move(p4type));
  }
}

void P4TupleT::Traverse(Visitor visit) const {
  for (std::size_t i = 0; i < members_.size(); ++i) {
    visit(i, members_.at(i));
  }
}

const P4Type& P4Type::operator[](absl::string_view name) const {
  return (*std::get<Box<P4StructT>>(variant_))[name];
}

const P4Type& P4Type::operator[](std::size_t index) const {
  return (*std::get<Box<P4TupleT>>(variant_))[index];
}

std::size_t P4Type::bitwidth() const {
  std::size_t bitwidth;
  std::visit(
      Overloaded{
          [&bitwidth](P4BitT p4type) { bitwidth = p4type.bitwidth(); },
          [&bitwidth](Box<P4StructT> p4type) { bitwidth = p4type->bitwidth(); },
          [&bitwidth](Box<P4TupleT> p4type) { bitwidth = p4type->bitwidth(); },
      },
      variant_);
  return bitwidth;
}

P4Data::P4Data(const P4Type& type, std::optional<uint8_t> init_val)
    : type_(type) {
  std::string prefix = "";
  std::size_t offset = 0;

  // Visit type nodes recursively to flatten type.
  std::function<void(absl::string_view, const P4Type&)> visit;
  visit = [&](absl::string_view name, const P4Type& p4type) -> void {
    std::visit(Overloaded{
                   [&](P4BitT bit_t) {
                     field_spec_[prefix + std::string{name}] = {
                         offset, bit_t.bitwidth()};
                     offset += bit_t.bitwidth();
                   },
                   [&](Box<P4StructT> struct_t) {
                     std::string prefix_backup = prefix;
                     prefix += (std::string{name} + "/");
                     struct_t->Traverse(visit);
                     prefix = prefix_backup;
                   },
                   [&](Box<P4TupleT> tuple_t) {
                     std::string prefix_backup = prefix;
                     prefix += (std::string{name} + "/");
                     tuple_t->Traverse(
                         [&](std::size_t index, const P4Type& p4type) -> void {
                           visit(std::to_string(index), p4type);
                         });
                     prefix = prefix_backup;
                   },
               },
               p4type.variant());
  };

  visit("", type);

  if (init_val.has_value()) {
    NewBuffer(init_val);
  }
}

BitField P4Data::operator[](absl::string_view path) {
  const auto& [offset, width] = field_spec_.at(path);
  if (buffer_ == nullptr) {
    NewBuffer();
  }
  return BitField(buffer_, offset, width);
}

void P4Data::NewBuffer(std::optional<uint8_t> init_val) {
  std::size_t size = (type_.bitwidth() + 7) / 8;
  buffer_ = std::make_shared<Buffer>(size, init_val);
}

}  // namespace p4buf
