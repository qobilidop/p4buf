#ifndef P4BUF_BUFFER_H_
#define P4BUF_BUFFER_H_

#include <cstddef>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "common.h"

namespace p4buf {

std::string get_greet(const std::string &who);

class RawBuffer {
 public:
  RawBuffer(const std::byte *data, bytewidth_t bytewidth)
      : data_(data), bytewidth_(bytewidth) {}

  RawBuffer(bytewidth_t byte_size) : bytewidth_(byte_size) {
    data_ = new std::byte[byte_size];
  }

 protected:
  const std::byte *data_;
  uint8_t bytewidth_;
};

}  // namespace p4buf

#endif  // P4BUF_BUFFER_H_
