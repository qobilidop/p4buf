#ifndef P4BUF_UTILITY_H_
#define P4BUF_UTILITY_H_

#include <memory>

namespace p4buf {

// This is a Rust-style smart pointer borrowed from
// https://www.foonathan.net/2022/05/recursive-variant-box/,
// useful for expressing recursive variants, like P4 data type specs.
template <typename T>
class Box {
 public:
  Box() = default;

  // Constructors from objects for convenience.
  Box(const T& obj) : ptr_(new T(obj)) {}
  Box(T&& obj) : ptr_(new T(std::move(obj))) {}

  // Copy semantics: copy the object pointed to.
  Box(const Box& other) : Box(*other.ptr_) {}
  Box& operator=(const Box& other) {
    *ptr_ = *other.ptr_;
    return *this;
  }

  // Move semantics: move the std::unique_ptr.
  // Note the issue pointed out in
  // https://www.foonathan.net/2022/05/recursive-variant-box/#aside-moving-boxes.
  Box(Box&& other) = default;
  Box& operator=(Box&& other) = default;

  // Destructs the std::unique_ptr automatically.
  ~Box() = default;

  // Propegates constness.
  T& operator*() { return *ptr_; }
  const T& operator*() const { return *ptr_; }
  T* operator->() { return ptr_.get(); }
  const T* operator->() const { return ptr_.get(); }

 private:
  std::unique_ptr<T> ptr_;
};

// Helpers for std::visit type-matching. See
// https://en.cppreference.com/w/cpp/utility/variant/visit.
template <class... Ts>
struct Overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

}  // namespace p4buf

#endif  // P4BUF_UTILITY_H_
