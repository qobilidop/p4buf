#include "p4buf/type_spec.h"

namespace p4buf {

BufferEditor GenerateEditor(const DataTS& spec) {
  BufferEditor editor;
  std::string prefix = "";
  std::size_t offset = 0;

  std::function<void(std::string, const DataTS&)> visit;
  visit = [&editor, &prefix, &offset, &visit](std::string name,
                                              const DataTS& spec) -> void {
    std::visit(
        Overloaded{[&editor, &prefix, &offset, &name](Box<BitTS> spec) {
                     editor.field_spec[prefix + name] = {offset, spec->width()};
                     offset += spec->width();
                   },
                   [&prefix, &name, &visit](Box<StructTS> spec) {
                     std::string prefix_backup = prefix;
                     prefix += (name + "/");
                     for (const auto& name : spec->member_names_) {
                       visit(name, spec->member_spec_[name]);
                     }
                     prefix = prefix_backup;
                   }},
        spec);
  };

  visit("", spec);

  return editor;
};

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
