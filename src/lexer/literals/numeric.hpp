#pragma once

#include <string_view>
#include <optional>

#include <utils/types.hpp>

namespace expr::lexer::literals {

struct Numeric {
  std::string_view text;
  d64 value;

  [[nodiscard]]
  static auto parse(const std::string_view source) -> std::optional<Numeric>;
};

} // namespace expr::lexer::literals
