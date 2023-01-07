#include "type_spec.h"

#include <gtest/gtest.h>

namespace p4buf {

TEST(TypeSpecTest, BitCtorEmpty) {
  BitTypeSpec spec;
  EXPECT_EQ(spec.get_bitwidth(), 0);
}

TEST(TypeSpecTest, BitCtorNormal) {
  BitTypeSpec spec0{0};
  EXPECT_EQ(spec0.get_bitwidth(), 0);

  BitTypeSpec spec42{42};
  EXPECT_EQ(spec42.get_bitwidth(), 42);
}

TEST(TypeSpecTest, BitCtorCopy) {
  BitTypeSpec spec{42};
  BitTypeSpec copy = spec;
  EXPECT_EQ(copy.get_bitwidth(), 42);
}

TEST(TypeSpecTest, StructCtorEmpty) {
  StructTypeSpec spec;
  EXPECT_EQ(spec.get_bitwidth(), 0);
}

TEST(TypeSpecTest, StructCtorInitList) {
  StructTypeSpec spec{
      {"a", 1},
      {"b", 2},
      {"c", 3},
      {"d", 4},
  };
  EXPECT_EQ(spec.get_bitwidth(), 10);
}

TEST(TypeSpecTest, StructCtorCopy) {
  StructTypeSpec spec{
      {"a", 1},
      {"b", 2},
      {"c", 3},
      {"d", 4},
  };
  StructTypeSpec copy = spec;
  EXPECT_EQ(copy.get_bitwidth(), 10);
}

TEST(TypeSpecTest, StructFlat) {
  StructTypeSpec spec;

  spec.add_bit("a", 0);
  auto spec_a = spec.view("a");
  EXPECT_EQ(spec_a->get_bitwidth(), 0);

  spec.add_bit("b", 1);
  auto spec_b = spec.view("b");
  EXPECT_EQ(spec_b->get_bitwidth(), 1);

  spec.add_bit("c", 8);
  auto spec_c = spec.view("c");
  EXPECT_EQ(spec_c->get_bitwidth(), 8);

  spec.add_bit("d", 9);
  auto spec_d = spec.view("d");
  EXPECT_EQ(spec_d->get_bitwidth(), 9);

  EXPECT_EQ(spec.get_bitwidth(), 18);
}

TEST(TypeSpecTest, StructNested) {
  StructTypeSpec spec_0;

  spec_0.add_bit("a", 0);
  spec_0.add_bit("b", 1);
  EXPECT_EQ(spec_0.get_bitwidth(), 1);

  StructTypeSpec spec_1;

  spec_1.add_struct("s0", spec_0);
  EXPECT_EQ(spec_1.view("s0")->get_bitwidth(), 1);
  EXPECT_EQ(spec_1.view("s0")->view("a")->get_bitwidth(), 0);
  EXPECT_EQ(spec_1.view("s0")->view("b")->get_bitwidth(), 1);

  spec_1.add_bit("c", 8);
  spec_1.add_bit("d", 9);
  EXPECT_EQ(spec_1.get_bitwidth(), 18);
}

}  // namespace p4buf
