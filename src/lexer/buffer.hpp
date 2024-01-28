#pragma once

#include <string_view>
#include <vector>

#include <lexer/literals/numeric.hpp>
#include <lexer/kind.hpp>

namespace expr::lexer {

struct Location {
  u32 line = 1;
  u32 column = 1;
  u32 idx = 0;
};

struct TokenData {
  Kind kind;
  Location loc;

  auto fmt() const -> std::string;
};

struct Buffer {
  // The `TokenBuffer` is the result of a successfull lexical analysis.
  // It contains several buffers that are indexable with a `Token`.
  // The primary buffer is the `tokens` that will contain what the `Kinds`
  // of lexed tokens along with the index into the `source` or one of the
  // side buffers.

  using TokenIdx = u32;
  using LiteralIdx = TokenIdx;

  std::vector<literals::Numeric> literals;
  std::vector<TokenData> tokens;

  std::string_view source;

  [[nodiscard, gnu::always_inline]]
  static auto from_source(const std::string_view source) -> Buffer {
    return { source };
  }
  
  auto push_token(const TokenData data) -> TokenIdx;
  auto push_literal(const literals::Numeric literal) -> LiteralIdx;

  [[nodiscard]]
  auto get_literal_idx(const TokenIdx idx) -> LiteralIdx;

protected: 
  Buffer(const std::string_view source) : source { source } {}
};

} // namespace expr::lexer
