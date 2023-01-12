#include "p4buf/buffer.h"

#include <gtest/gtest.h>

#include "absl/strings/str_format.h"

namespace p4buf {

testing::AssertionResult buf_eq(const Buffer& got, const Buffer& want) {
  if (got.size() != want.size()) {
    return testing::AssertionFailure()
           << absl::StrFormat("Buffer sizes differ. Got: %u. Want: %u.",
                              got.size(), want.size());
    ;
  }
  for (std::size_t i = 0; i < got.size(); ++i) {
    if (got.data()[i] != want.data()[i]) {
      return testing::AssertionFailure() << absl::StrFormat(
                 "The %uth bytes differ. Got: %0#04x. Want: %0#04x.", i,
                 got.data()[i], want.data()[i]);
      ;
    }
  }
  return testing::AssertionSuccess();
}

TEST(BufferTest, Buffer) {
  Buffer buffer_uninit(4);
  EXPECT_EQ(buffer_uninit.size(), 4);
  EXPECT_FALSE(buf_eq(buffer_uninit, {0, 0, 0, 0}));

  Buffer buffer(4, 1);
  EXPECT_TRUE(buf_eq(buffer, {1, 1, 1, 1}));

  buffer.data()[1] = std::byte{2};
  buffer.data()[2] = std::byte{3};
  buffer.data()[3] = std::byte{4};
  EXPECT_TRUE(buf_eq(buffer, {1, 2, 3, 4}));

  std::vector<uint8_t> bytes{5, 6, 7, 8};
  std::memcpy(buffer.data(), bytes.data(), 4);
  EXPECT_TRUE(buf_eq(buffer, {5, 6, 7, 8}));

  Buffer buffer_from_bytes{1, 2, 3, 4};
  EXPECT_TRUE(buf_eq(buffer_from_bytes, {1, 2, 3, 4}));
}

TEST(BufferTest, BufferViewCtor) {
  auto buffer = std::make_shared<Buffer>(4, 0);
  BitField bv0(buffer, 0, 8);
  EXPECT_EQ(bv0.buffer()->size(), 4);
  EXPECT_TRUE(buf_eq(*bv0.buffer(), {0, 0, 0, 0}));
  EXPECT_EQ(bv0.offset(), 0);
  EXPECT_EQ(bv0.width(), 8);

  BitField bv1({1, 2, 3, 4});
  EXPECT_EQ(bv1.buffer()->size(), 4);
  EXPECT_TRUE(buf_eq(*bv1.buffer(), {1, 2, 3, 4}));
  EXPECT_EQ(bv1.offset(), 0);
  EXPECT_EQ(bv1.width(), 32);

  BitField bv2({1, 2, 3, 4}, 8);
  EXPECT_EQ(bv2.buffer()->size(), 4);
  EXPECT_TRUE(buf_eq(*bv2.buffer(), {1, 2, 3, 4}));
  EXPECT_EQ(bv2.offset(), 24);
  EXPECT_EQ(bv2.width(), 8);
}

TEST(BufferTest, BufferViewWriteAlignedToAligned) {
  auto buffer_to_write = std::make_shared<Buffer>(4, 0);
  BitField bv_to_write(buffer_to_write, 8, 16);

  auto buffer_of_value = std::make_shared<Buffer>(
      std::initializer_list<uint8_t>{0x1a, 0x2b, 0x3c, 0x4d});

  // Write the left 2 bytes.
  bv_to_write.Write({buffer_of_value, 0, 16});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x1a, 0x2b, 0}));

  // Write the middle 2 bytes.
  bv_to_write.Write({buffer_of_value, 8, 16});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x2b, 0x3c, 0}));

  // Write the right 2 bytes.
  bv_to_write.Write({buffer_of_value, 16, 16});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x3c, 0x4d, 0}));

  // Write just 1 byte.
  bv_to_write.Write({buffer_of_value, 0, 8});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x1a, 0x4d, 0}));

  // Try to write more than 2 bytes.
  bv_to_write.Write({buffer_of_value, 8, 24});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x2b, 0x3c, 0}));
}

TEST(BufferTest, BufferViewWriteAlignedToUnaligned) {
  auto buffer_to_write = std::make_shared<Buffer>(4, 0);
  // 0b0000'0000, 0b0000'0000, 0b0000'0000, 0
  //         ^^^    ^^^^ ^^^^    ^^
  BitField bv_to_write(buffer_to_write, 5, 13);

  auto buffer_of_value =
      std::make_shared<Buffer>(std::initializer_list<uint8_t>{
          0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101});

  // Write the left 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //   ^^^^ ^^^^    ^^^^ ^
  bv_to_write.Write({buffer_of_value, 0, 13});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0000, 0b1101'0001, 0b0100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write the middle 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                ^^^^ ^^^^    ^^^^ ^
  bv_to_write.Write({buffer_of_value, 8, 13});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0001, 0b0101'1001, 0b1100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write the right 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                             ^^^^ ^^^^    ^^^^ ^
  bv_to_write.Write({buffer_of_value, 16, 13});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0001, 0b1110'0010, 0b0100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write just 6 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //   ^^^^ ^^
  bv_to_write.Write({buffer_of_value, 0, 6});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0000, 0b1100'0010, 0b0100'0000, 0}));
  //                          ^^^    ^^^- ----    --

  // Try to write more than 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                ^^^^ ^^^^    ^^^^ ^---    ---- -
  bv_to_write.Write({buffer_of_value, 8, 21});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0001, 0b0101'1001, 0b1100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^
}

TEST(BufferTest, BufferViewWriteUnalignedToAligned) {
  auto buffer_to_write = std::make_shared<Buffer>(4, 0);
  BitField bv_to_write(buffer_to_write, 8, 16);

  auto buffer_of_value = std::make_shared<Buffer>(
      std::initializer_list<uint8_t>{0b000'0001'1, 0b010'0010'1, 0b011'0011'1,
                                     0b100'0100'1, 0b101'0000'0});

  // Write the left 2 bytes, with a 3-bit offset.
  bv_to_write.Write({buffer_of_value, 3, 16});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x1a, 0x2b, 0}));

  // Write the middle 2 bytes, with a 3-bit offset.
  bv_to_write.Write({buffer_of_value, 11, 16});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x2b, 0x3c, 0}));

  // Write the right 2 bytes, with a 3-bit offset.
  bv_to_write.Write({buffer_of_value, 19, 16});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x3c, 0x4d, 0}));

  // Write just 1 byte + 4 bits.
  bv_to_write.Write({buffer_of_value, 3, 12});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x1a, 0x2d, 0}));

  // Try to write more than 2 bytes.
  bv_to_write.Write({buffer_of_value, 11, 21});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x2b, 0x3c, 0}));
}

TEST(BufferTest, BufferViewWriteUnalignedToUnaligned) {
  auto buffer_to_write = std::make_shared<Buffer>(4, 0);
  // 0b0000'0000, 0b0000'0000, 0b0000'0000, 0
  //         ^^^    ^^^^ ^^^^    ^^
  BitField bv_to_write(buffer_to_write, 5, 13);

  auto buffer_of_value =
      std::make_shared<Buffer>(std::initializer_list<uint8_t>{
          0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101});

  // Write 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //      ^ ^^^^    ^^^^ ^^^^
  bv_to_write.Write({buffer_of_value, 3, 13});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0110, 0b1000'1010, 0b1100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //          ^^    ^^^^ ^^^^    ^^^
  bv_to_write.Write({buffer_of_value, 6, 13});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0100, 0b0101'0110, 0b0100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                 ^^^ ^^^^    ^^^^ ^^
  bv_to_write.Write({buffer_of_value, 9, 13});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0010, 0b1011'0011, 0b1100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write 7 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                                           ^^^ ^^^^
  bv_to_write.Write({buffer_of_value, 25, 7});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0100, 0b1101'0011, 0b1100'0000, 0}));
  //                          ^^^    ^^^^ ----    --

  // Try to write more than 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                 ^^^ ^^^^    ^^^^ ^^--    ---- ----
  bv_to_write.Write({buffer_of_value, 9, 23});
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0010, 0b1011'0011, 0b1100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^
}

}  // namespace p4buf
