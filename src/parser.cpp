#include "include/parser.hpp"

namespace Parser {

[[nodiscard]] auto parse_lit_chr(Parser &p) -> Ast::Node * {
  auto _ = defer([&p]() { next_t(p); });

  return nullptr;
}

[[nodiscard]] auto parse_ident(Parser &p) -> Ast::Node * {
  auto _ = defer([&p]() { next_t(p); });
  return new Ast::Node{Ast::Ident{std::move(curr_t(p))}}; // NOLINT
}
} // namespace Parser