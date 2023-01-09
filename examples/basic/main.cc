#include "fmt/core.h"
#include "p4buf/p4buf.h"

int main(int argc, char *argv[]) {
  // Express the following P4 data types:
  //
  // struct s0_t {
  //     bit<1> a;
  //     bit<2> b;
  // }
  //
  // struct s1_t {
  //     s0_t s0;
  //     bit<3> c;
  //     bit<4> d;
  // }

  p4buf::StructTypeSpec s0_t;
  s0_t.add("a", 1);
  s0_t.add("b", 2);

  p4buf::StructTypeSpec s1_t;
  s1_t.add("s0", s0_t);
  s1_t.add("c", 3);
  s1_t.add("d", 4);

  fmt::print("P4 data type spec:\n");
  s1_t.print();

  fmt::print("\nNow let's create some buffers and set all fields to 1.\n");

  for (auto layout : {p4buf::Layout::compact, p4buf::Layout::byte_aligned}) {
    fmt::print("\n");
    p4buf::Schema schema{s1_t.copy_ptr(), layout};
    schema.print();
  }

  return 0;
}