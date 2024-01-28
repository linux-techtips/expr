#include <format>

#include <lexer/buffer.hpp>

#include <utils/assert.hpp>

namespace expr::lexer {

auto TokenData::fmt() const -> std::string {
  return std::string { kind.fmt() };
}

auto Buffer::push_token(const TokenData data) -> TokenIdx {
  tokens.push_back(data);
  return tokens.size() - 1;
}

auto Buffer::push_literal(const literals::Numeric literal) -> LiteralIdx {
  literals.push_back(literal);
  return literals.size() - 1;
}

} // namespace expr::lexer
