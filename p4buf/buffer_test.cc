#include "p4buf/buffer.h"

#include <gtest/gtest.h>

#include "absl/strings/str_format.h"

namespace p4buf {

testing::AssertionResult buf_eq(const Buffer& got, const Buffer& want) {
  if (got.size() != want.size()) {
    return testing::AssertionFailure()
           << absl::StrFormat("Buffer size differs. Got: %u. Want: %u.",
                              got.size(), want.size());
    ;
  }
  for (std::size_t i = 0; i < got.size(); ++i) {
    if (got.data()[i] != want.data()[i]) {
      return testing::AssertionFailure() << absl::StrFormat(
                 "The %uth byte differs. Got: %0#04x. Want: %0#04x.", i,
                 got.data()[i], want.data()[i]);
      ;
    }
  }
  return testing::AssertionSuccess();
}

TEST(BufferTest, Buffer) {
  // Create a buffer from allocated memory.
  std::unique_ptr<std::byte[]> data(new std::byte[4]);
  Buffer b1(std::move(data), 4);
  EXPECT_EQ(b1.size(), 4);

  // Create a buffer of given size, but uninitialized.
  Buffer b2(4);
  EXPECT_EQ(b2.size(), 4);

  // Create a buffer of given size, and initialized.
  Buffer b3(4, 0);
  EXPECT_TRUE(buf_eq(b3, {0, 0, 0, 0}));

  // Create a buffer from given bytes.
  Buffer b4{0, 1, 2, 3};
  EXPECT_TRUE(buf_eq(b4, {0, 1, 2, 3}));

  // Edit individual byte.
  Buffer b5(4);
  b5[0] = std::byte{0};
  b5[1] = std::byte{1};
  b5[2] = std::byte{2};
  b5[3] = std::byte{3};
  EXPECT_TRUE(buf_eq(b5, {0, 1, 2, 3}));

  // Edit the whole buffer.
  Buffer b6(4);
  std::array<uint8_t, 4> bytes{0, 1, 2, 3};
  std::memcpy(b6.data(), bytes.data(), 4);
  EXPECT_TRUE(buf_eq(b6, {0, 1, 2, 3}));
}

TEST(BufferTest, BitFieldCtor) {
  // Create bit field from existing buffer.
  auto buffer = std::make_shared<Buffer>(4, 0);
  BitField bf1(buffer, 0, 8);
  EXPECT_TRUE(buf_eq(*bf1.buffer(), {0, 0, 0, 0}));
  EXPECT_EQ(bf1.offset(), 0);
  EXPECT_EQ(bf1.width(), 8);

  // Create bit field from bytes.
  BitField bf2({0, 1, 2, 3});
  EXPECT_TRUE(buf_eq(*bf2.buffer(), {0, 1, 2, 3}));
  EXPECT_EQ(bf2.offset(), 0);
  EXPECT_EQ(bf2.width(), 32);

  // Create bit field from uint8_t.
  BitField bf3(uint8_t{0x0a});
  EXPECT_TRUE(buf_eq(*bf3.buffer(), {0x0a}));
  EXPECT_EQ(bf3.offset(), 0);
  EXPECT_EQ(bf3.width(), 8);

  // Create bit field from uint16_t.
  BitField bf4(uint16_t{0x0a1b});
  EXPECT_TRUE(buf_eq(*bf4.buffer(), {0x0a, 0x1b}));
  EXPECT_EQ(bf4.offset(), 0);
  EXPECT_EQ(bf4.width(), 16);

  // Create bit field from uint32_t.
  BitField bf5(uint32_t{0x0a1b2c3d});
  EXPECT_TRUE(buf_eq(*bf5.buffer(), {0x0a, 0x1b, 0x2c, 0x3d}));
  EXPECT_EQ(bf5.offset(), 0);
  EXPECT_EQ(bf5.width(), 32);

  // Create bit field from uint64_t.
  BitField bf6(uint64_t{0x0a1b2c3d0a1b2c3d});
  EXPECT_TRUE(
      buf_eq(*bf6.buffer(), {0x0a, 0x1b, 0x2c, 0x3d, 0x0a, 0x1b, 0x2c, 0x3d}));
  EXPECT_EQ(bf6.offset(), 0);
  EXPECT_EQ(bf6.width(), 64);
}

TEST(BufferTest, BitFieldWriteAlignedToAligned) {
  auto buffer_to_write = std::make_shared<Buffer>(4, 0);
  BitField bv_to_write(buffer_to_write, 8, 16);

  auto buffer_of_value = std::make_shared<Buffer>(
      std::initializer_list<uint8_t>{0x1a, 0x2b, 0x3c, 0x4d});

  // Write the left 2 bytes.
  bv_to_write = {buffer_of_value, 0, 16};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x1a, 0x2b, 0}));

  // Write the middle 2 bytes.
  bv_to_write = {buffer_of_value, 8, 16};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x2b, 0x3c, 0}));

  // Write the right 2 bytes.
  bv_to_write = {buffer_of_value, 16, 16};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x3c, 0x4d, 0}));

  // Write just 1 byte.
  // bv_to_write = {buffer_of_value, 0, 8};
  // EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x1a, 0x4d, 0}));

  // Try to write more than 2 bytes.
  // bv_to_write = {buffer_of_value, 8, 24};
  // EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x2b, 0x3c, 0}));
}

TEST(BufferTest, BitFieldWriteAlignedToUnaligned) {
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
  bv_to_write = {buffer_of_value, 0, 13};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0000, 0b1101'0001, 0b0100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write the middle 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                ^^^^ ^^^^    ^^^^ ^
  bv_to_write = {buffer_of_value, 8, 13};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0001, 0b0101'1001, 0b1100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write the right 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                             ^^^^ ^^^^    ^^^^ ^
  bv_to_write = {buffer_of_value, 16, 13};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0001, 0b1110'0010, 0b0100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write just 6 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //   ^^^^ ^^
  // bv_to_write = {buffer_of_value, 0, 6};
  // EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
  //                    {0b0000'0000, 0b1100'0010, 0b0100'0000, 0}));
  // //                          ^^^    ^^^- ----    --

  // Try to write more than 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                ^^^^ ^^^^    ^^^^ ^---    ---- -
  // bv_to_write = {buffer_of_value, 8, 21};
  // EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
  //                    {0b0000'0001, 0b0101'1001, 0b1100'0000, 0}));
  // //                          ^^^    ^^^^ ^^^^    ^^
}

TEST(BufferTest, BitFieldWriteUnalignedToAligned) {
  auto buffer_to_write = std::make_shared<Buffer>(4, 0);
  BitField bv_to_write(buffer_to_write, 8, 16);

  auto buffer_of_value = std::make_shared<Buffer>(
      std::initializer_list<uint8_t>{0b000'0001'1, 0b010'0010'1, 0b011'0011'1,
                                     0b100'0100'1, 0b101'0000'0});

  // Write the left 2 bytes, with a 3-bit offset.
  bv_to_write = {buffer_of_value, 3, 16};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x1a, 0x2b, 0}));

  // Write the middle 2 bytes, with a 3-bit offset.
  bv_to_write = {buffer_of_value, 11, 16};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x2b, 0x3c, 0}));

  // Write the right 2 bytes, with a 3-bit offset.
  bv_to_write = {buffer_of_value, 19, 16};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x3c, 0x4d, 0}));

  // Write just 1 byte + 4 bits.
  // bv_to_write = {buffer_of_value, 3, 12};
  // EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x1a, 0x2d, 0}));

  // Try to write more than 2 bytes.
  // bv_to_write = {buffer_of_value, 11, 21};
  // EXPECT_TRUE(buf_eq(*bv_to_write.buffer(), {0, 0x2b, 0x3c, 0}));
}

TEST(BufferTest, BitFieldWriteUnalignedToUnaligned) {
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
  bv_to_write = {buffer_of_value, 3, 13};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0110, 0b1000'1010, 0b1100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //          ^^    ^^^^ ^^^^    ^^^
  bv_to_write = {buffer_of_value, 6, 13};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0100, 0b0101'0110, 0b0100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                 ^^^ ^^^^    ^^^^ ^^
  bv_to_write = {buffer_of_value, 9, 13};
  EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
                     {0b0000'0010, 0b1011'0011, 0b1100'0000, 0}));
  //                          ^^^    ^^^^ ^^^^    ^^

  // Write 7 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                                           ^^^ ^^^^
  // bv_to_write = {buffer_of_value, 25, 7};
  // EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
  //                    {0b0000'0100, 0b1101'0011, 0b1100'0000, 0}));
  // //                          ^^^    ^^^^ ----    --

  // Try to write more than 13 bits.
  // 0b0001'1010, 0b0010'1011, 0b0011'1100, 0b0100'1101
  //                 ^^^ ^^^^    ^^^^ ^^--    ---- ----
  // bv_to_write = {buffer_of_value, 9, 23};
  // EXPECT_TRUE(buf_eq(*bv_to_write.buffer(),
  //                    {0b0000'0010, 0b1011'0011, 0b1100'0000, 0}));
  // //                          ^^^    ^^^^ ^^^^    ^^
}

}  // namespace p4buf
