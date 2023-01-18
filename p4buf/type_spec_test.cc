#include "p4buf/type_spec.h"

#include <gtest/gtest.h>

namespace p4buf {

TEST(TypeSpecTest, BitTS) {
  auto ts = BitTS{42};
  EXPECT_EQ(ts.width(), 42);
}

TEST(TypeSpecTest, StructTS) {
  auto ts1 = StructTS{
      {"a", BitTS{1}},
      {"b", BitTS{2}},
      {"c", BitTS{3}},
      {"d", BitTS{4}},
  };
  EXPECT_EQ(ts1.width(), 10);

  auto ts2 = StructTS{
      {"s0",
       StructTS{
           {"a", BitTS{1}},
           {"b", BitTS{2}},
       }},
      {"c", BitTS{3}},
      {"d", BitTS{4}},
  };
  EXPECT_EQ(ts2.width(), 10);

  auto ts3 = StructTS{
      {"s1", ts1},
      {"s2", ts2},
  };
  EXPECT_EQ(ts3.width(), 20);

  EXPECT_EQ(ts3.Get<StructTS>("s1").width(), 10);
  EXPECT_EQ(ts3.Get<StructTS>("s2").Get<StructTS>("s0").Get<BitTS>("a").width(),
            1);
}

}  // namespace p4buf
