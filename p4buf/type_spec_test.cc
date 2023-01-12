#include "p4buf/type_spec.h"

#include <gtest/gtest.h>

namespace p4buf {

TEST(TypeSpecTest, BitCtorEmpty) {
  BitTypeSpec ts;
  EXPECT_EQ(ts.bitwidth(), 0);
}

TEST(TypeSpecTest, BitCtorNormal) {
  BitTypeSpec ts{42};
  EXPECT_EQ(ts.bitwidth(), 42);
}

TEST(TypeSpecTest, BitCtorCopy) {
  BitTypeSpec ts0{42};
  BitTypeSpec ts1 = ts0;
  EXPECT_EQ(ts1.bitwidth(), ts0.bitwidth());
}

TEST(TypeSpecTest, StructCtorEmpty) {
  StructTypeSpec ts;
  EXPECT_EQ(ts.bitwidth(), 0);
}

TEST(TypeSpecTest, StructBasicOperations) {
  StructTypeSpec ts;
  ts.add("a", 1);
  ts.add("b", 2);
  ts.add("c", 3);
  ts.add("d", 4);
  EXPECT_EQ(ts.bitwidth(), 10);
}

TEST(TypeSpecTest, StructCtorCopy) {
  StructTypeSpec ts0;
  ts0.add("a", 1);
  ts0.add("b", 2);
  ts0.add("c", 3);
  ts0.add("d", 4);
  StructTypeSpec ts1 = ts0;
  EXPECT_EQ(ts1.bitwidth(), ts0.bitwidth());
}

TEST(TypeSpecTest, StructFlat) {
  StructTypeSpec ts;

  ts.add("a", 1);
  auto ts_a = ts.view("a");
  EXPECT_EQ(ts_a->bitwidth(), 1);

  ts.add("b", 2);
  auto ts_b = ts.view("b");
  EXPECT_EQ(ts_b->bitwidth(), 2);

  ts.add("c", 3);
  auto ts_c = ts.view("c");
  EXPECT_EQ(ts_c->bitwidth(), 3);

  ts.add("d", 4);
  auto ts_d = ts.view("d");
  EXPECT_EQ(ts_d->bitwidth(), 4);

  EXPECT_EQ(ts.bitwidth(), 10);
}

TEST(TypeSpecTest, StructNested) {
  StructTypeSpec ts_0;

  ts_0.add("a", 1);
  ts_0.add("b", 2);
  EXPECT_EQ(ts_0.bitwidth(), 3);

  StructTypeSpec ts_1;

  ts_1.add("s0", ts_0);
  EXPECT_EQ(ts_1.view("s0")->bitwidth(), 3);
  EXPECT_EQ(ts_1.view("s0")->view("a")->bitwidth(), 1);
  EXPECT_EQ(ts_1.view("s0")->view("b")->bitwidth(), 2);

  ts_1.add("c", 3);
  ts_1.add("d", 4);
  EXPECT_EQ(ts_1.bitwidth(), 10);
}

}  // namespace p4buf
