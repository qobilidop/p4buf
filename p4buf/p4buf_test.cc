#include "p4buf/p4buf.h"

#include <gtest/gtest.h>

namespace p4buf {

TEST(TypeSpecTest, BitCtorEmpty) {
  BitTypeSpec ts;
  EXPECT_EQ(ts.get_bitwidth(), 0);
}

TEST(TypeSpecTest, BitCtorNormal) {
  BitTypeSpec ts{42};
  EXPECT_EQ(ts.get_bitwidth(), 42);
}

TEST(TypeSpecTest, BitCtorCopy) {
  BitTypeSpec ts0{42};
  BitTypeSpec ts1 = ts0;
  EXPECT_EQ(ts1.get_bitwidth(), ts0.get_bitwidth());
}

TEST(TypeSpecTest, StructCtorEmpty) {
  StructTypeSpec ts;
  EXPECT_EQ(ts.get_bitwidth(), 0);
}

TEST(TypeSpecTest, StructBasicOperations) {
  StructTypeSpec ts;
  ts.add("a", 1);
  ts.add("b", 2);
  ts.add("c", 3);
  ts.add("d", 4);
  EXPECT_EQ(ts.get_bitwidth(), 10);
}

TEST(TypeSpecTest, StructCtorCopy) {
  StructTypeSpec ts0;
  ts0.add("a", 1);
  ts0.add("b", 2);
  ts0.add("c", 3);
  ts0.add("d", 4);
  StructTypeSpec ts1 = ts0;
  EXPECT_EQ(ts1.get_bitwidth(), ts0.get_bitwidth());
}

TEST(TypeSpecTest, StructFlat) {
  StructTypeSpec ts;

  ts.add("a", 1);
  auto ts_a = ts.view("a");
  EXPECT_EQ(ts_a->get_bitwidth(), 1);

  ts.add("b", 2);
  auto ts_b = ts.view("b");
  EXPECT_EQ(ts_b->get_bitwidth(), 2);

  ts.add("c", 3);
  auto ts_c = ts.view("c");
  EXPECT_EQ(ts_c->get_bitwidth(), 3);

  ts.add("d", 4);
  auto ts_d = ts.view("d");
  EXPECT_EQ(ts_d->get_bitwidth(), 4);

  EXPECT_EQ(ts.get_bitwidth(), 10);
}

TEST(TypeSpecTest, StructNested) {
  StructTypeSpec ts_0;

  ts_0.add("a", 1);
  ts_0.add("b", 2);
  EXPECT_EQ(ts_0.get_bitwidth(), 3);

  StructTypeSpec ts_1;

  ts_1.add("s0", ts_0);
  EXPECT_EQ(ts_1.view("s0")->get_bitwidth(), 3);
  EXPECT_EQ(ts_1.view("s0")->view("a")->get_bitwidth(), 1);
  EXPECT_EQ(ts_1.view("s0")->view("b")->get_bitwidth(), 2);

  ts_1.add("c", 3);
  ts_1.add("d", 4);
  EXPECT_EQ(ts_1.get_bitwidth(), 10);
}

}  // namespace p4buf
