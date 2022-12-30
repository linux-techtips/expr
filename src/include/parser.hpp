#pragma once

#include "ast.hpp"
#include "tokenizer.hpp"

namespace Parser {

struct Parser {
  bool done{false};
  usize pos{0};

  Ast::Node *ast{};

  char *contents{};
  std::vector<Tok::Token> toks{};

  [[nodiscard]] static inline auto from(Tokenizer::Reader &r)
      -> Result<Parser, std::string> {
    if (r.contents == nullptr || r.toks.size() < 1) {
      return {std::string{"Failed to create parser."}};
    }
    return {Parser{.contents = r.contents, .toks = std::move(r.toks)}};
  } // namespace Parser
};

[[nodiscard]] inline auto curr_t(Parser &p) -> Tok::Token & {
  return p.toks[p.pos];
}

inline auto next_t(Parser &p) -> void {
  if (++p.pos >= p.toks.size()) {
    p.done = true;
  }
}



auto parse(Parser &p) -> void;

[[nodiscard]] auto parse_toplevel(Parser &p) -> Ast::Node *;

[[nodiscard]] auto parse_expr(Parser &p) -> Ast::Node *;
[[nodiscard]] auto parse_primary_expr(Parser &p) -> Ast::Node *;
[[nodiscard]] auto parse_paren_expr(Parser &p) -> Ast::Node *;
[[nodiscard]] auto parse_expr_decl(Parser &p) -> Ast::Node *;

[[nodiscard]] auto parse_bin_expr(Parser &p) -> Ast::Node *;

[[nodiscard]] auto parse_lit_int(Parser &p) -> Ast::Node *;
[[nodiscard]] auto parse_lit_dec(Parser &p) -> Ast::Node *;
[[nodiscard]] auto parse_lit_chr(Parser &p) -> Ast::Node *;

[[nodiscard]] auto parse_ident(Parser &p) -> Ast::Node *;

} // namespace Parser