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
    {"a", P4BitT{1}},       //   bit<1> a;
    {"b", P4BitT{2}},       //   bit<2> b;
    {"s", P4StructT{        //   struct {
      {"c", P4BitT{3}},     //     bit<3> c;
      {"d", P4BitT{4}},     //     bit<4> d;
    }},                     //   } s;
    {"t", P4TupleT{         //   tuple<
      P4BitT{5},            //     bit<5>,
      P4BitT{6},            //     bit<6>,
    }},                     //   > t;
  });                       // }
  // clang-format on

  // Create a P4 data object with the specified type.
  // Initialize all bytes to 0.
  P4Data p4data(p4type, 0);

  // Edit named fields.
  p4data["/a"] = uint8_t{1};
  p4data["/b"] = uint8_t{1};
  p4data["/s/c"] = uint8_t{1};
  p4data["/s/d"] = p4data["/a"];
  p4data["/t/0"] = uint8_t{1};
  p4data["/t/1"] = p4data["/b"];

  // Now the underlying buffer becomes:
  // 10100100 01000010 00001000
  // abbcccdd dd.t[0]. .t[1]
  PrintBuffer(*p4data.buffer());

  return 0;
}
