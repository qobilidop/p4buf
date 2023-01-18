#include "p4buf/type_spec.h"

namespace p4buf {

StructTS::StructTS(
    std::initializer_list<std::tuple<std::string, DataTS>> specs) {
  for (auto& [name, spec] : specs) {
    std::visit(
        Overloaded{[this](Box<BitTS> spec) { width_ += spec->width(); },
                   [this](Box<StructTS> spec) { width_ += spec->width(); }

        },
        spec);
    member_names_.emplace_back(name);
    member_spec_.emplace(name, spec);
  }
};

}  // namespace p4buf
