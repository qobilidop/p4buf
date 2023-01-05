#include "p4buf.h"

#include <gtest/gtest.h>

namespace p4buf {

TEST(HelloTest, Basic) { EXPECT_EQ(get_greet("World"), "Hello World"); }

}  // namespace p4buf
