#pragma once

#include <lexer/buffer.hpp>

#include <utils/assert.hpp>

namespace expr::lexer {

[[nodiscard]]
auto lex(const std::string_view source) -> Buffer;

} // namespace expr::lexer
