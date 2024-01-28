#pragma once

#include <utility>

namespace {

template <typename Fn>
struct Defer {
  Fn fn;

  constexpr ~Defer() { fn(); }
};

struct __defer {};

template <typename Fn>
[[gnu::always_inline]]
constexpr auto operator + ([[gnu::unused]] __defer, Fn&& fn) -> Defer<Fn> {
  return { std::forward<Fn>(fn) };
}

} // namespace hidden 

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

#define defer auto CONCAT(__defer_, __COUNTER__) = ::__defer {} + [&] mutable
