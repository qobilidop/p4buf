#include <bitset>
#include <iostream>

#include "p4buf/p4data.h"

using namespace p4buf;

void PrintBuffer(const p4buf::Buffer& buffer) {
  std::cout << "bytes:";
  for (std::size_t i = 0; i < buffer.size(); ++i) {
    std::cout << " " << std::bitset<8>(std::to_integer<int>(buffer.at(i)));
  }
  std::cout << "\n";
}

int main(int argc, char* argv[]) {
  // Specify a P4 type.
  // clang-format off
  P4Type p4type(P4StructT{  // struct {
    {"s0", P4StructT{       //   struct {
      {"a", P4BitT{1}},     //     bit<1> a;
      {"b", P4BitT{2}},     //     bit<2> b;
    }},                     //   } s0;
    {"c", P4BitT{3}},       //   bit<3> c;
    {"d", P4BitT{4}},       //   bit<4> d;
  });                       // }
  // clang-format on

  // Create a P4 data object with the specified type.
  // Initialize all bytes to 0.
  P4Data p4data(p4type, 0);

  // Edit named fields.
  p4data["/s0/a"] = uint8_t{1};
  p4data["/s0/b"] = uint8_t{1};
  p4data["/c"] = uint8_t{1};
  p4data["/d"] = p4data["/c"];

  // Now the underlying buffer becomes:
  // 10100100 01000000
  // abbcccdd dd
  PrintBuffer(*p4data.buffer());

  return 0;
}
