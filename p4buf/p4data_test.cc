#include "p4buf/p4data.h"

#include <gtest/gtest.h>

namespace p4buf {

TEST(P4DataTest, P4TypeBasic) {
  auto bit_t = P4BitT{42};
  EXPECT_EQ(bit_t.bitwidth(), 42);

  auto struct_t = P4StructT{
      {"a", P4BitT{1}},
      {"b", P4BitT{2}},
      {"c", P4BitT{3}},
      {"d", P4BitT{4}},
  };
  EXPECT_EQ(struct_t.bitwidth(), 10);
  EXPECT_EQ(struct_t["a"].bitwidth(), 1);
  EXPECT_EQ(struct_t["b"].bitwidth(), 2);
  EXPECT_EQ(struct_t["c"].bitwidth(), 3);
  EXPECT_EQ(struct_t["d"].bitwidth(), 4);

  auto tuple_t = P4TupleT{
      P4BitT{1},
      P4BitT{2},
      P4BitT{3},
      P4BitT{4},
  };
  EXPECT_EQ(tuple_t.bitwidth(), 10);
  EXPECT_EQ(tuple_t[0].bitwidth(), 1);
  EXPECT_EQ(tuple_t[1].bitwidth(), 2);
  EXPECT_EQ(tuple_t[2].bitwidth(), 3);
  EXPECT_EQ(tuple_t[3].bitwidth(), 4);
}

TEST(P4DataTest, P4TypeNested) {
  auto struct_struct_t = P4StructT{
      {"a", P4BitT{1}},
      {"b", P4BitT{2}},
      {"struct_t",
       P4StructT{
           {"c", P4BitT{3}},
           {"d", P4BitT{4}},
       }},
  };
  EXPECT_EQ(struct_struct_t.bitwidth(), 10);
  EXPECT_EQ(struct_struct_t["struct_t"].bitwidth(), 7);
  EXPECT_EQ(struct_struct_t["struct_t"]["d"].bitwidth(), 4);

  auto struct_tuple_t = P4StructT{
      {"a", P4BitT{1}},
      {"b", P4BitT{2}},
      {"tuple_t",
       P4TupleT{
           P4BitT{3},
           P4BitT{4},
       }},
  };
  EXPECT_EQ(struct_tuple_t.bitwidth(), 10);
  EXPECT_EQ(struct_tuple_t["tuple_t"].bitwidth(), 7);
  EXPECT_EQ(struct_tuple_t["tuple_t"][1].bitwidth(), 4);

  auto tuple_struct_t = P4TupleT{
      P4BitT{1},
      P4BitT{2},
      P4StructT{
          {"c", P4BitT{3}},
          {"d", P4BitT{4}},
      },
  };
  EXPECT_EQ(tuple_struct_t.bitwidth(), 10);
  EXPECT_EQ(tuple_struct_t[2].bitwidth(), 7);
  EXPECT_EQ(tuple_struct_t[2]["d"].bitwidth(), 4);

  auto tuple_tuple_t = P4TupleT{
      P4BitT{1},
      P4BitT{2},
      P4TupleT{
          P4BitT{3},
          P4BitT{4},
      },
  };
  EXPECT_EQ(tuple_tuple_t.bitwidth(), 10);
  EXPECT_EQ(tuple_tuple_t[2].bitwidth(), 7);
  EXPECT_EQ(tuple_tuple_t[2][1].bitwidth(), 4);
}

}  // namespace p4buf
