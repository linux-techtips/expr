#pragma once

namespace expr::lexer::pred {

[[nodiscard]]
constexpr auto is_alpha(char c) -> bool {
  return (c | 0x20) - 'a' < 26;
}

[[nodiscard]]
constexpr auto is_digit(char c) -> bool {
  return c - '0' < 10;
}

[[nodiscard]]
constexpr auto is_alnum(char c) -> bool {
  return is_alpha(c) || is_digit(c);
}

[[nodiscard]]
constexpr auto is_horizontal_whitespace(char c) -> bool {
  return c == ' ' || c == '\t';
}

[[nodiscard]]
constexpr auto is_vertical_whitespace(char c) -> bool {
  return c == '\n' || c == '\r';
}

[[nodiscard]]
constexpr auto is_insignificant_whitespace(char c) -> bool {
  return is_horizontal_whitespace(c) || c == '\r';
}

} // namespace expr::lexer::pred
