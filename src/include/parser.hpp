#pragma once

#include "ast.hpp"
#include "tok.hpp"

namespace Parser {

#define LIT_CASE                                                               \
  Tok::LitInt : case Tok::LitDec:                                              \
  case Tok::LitChr:                                                            \
  case Tok::LitStr

struct ParseError;
struct Parser {
  using Self = Parser;

  const char *file_path{};
  const char *file_name{};

  Ast::Node *curr_n{};
  Ast::Node *curr_fn{};

  usize pos{};
  Tokenizer::Reader reader{};

  [[nodiscard]] static auto from(const char *fname) -> Result<Self, ParseError>;
};

using OptToken = Option<std::reference_wrapper<Tok::Token>>;

auto curr_t(Parser &p) -> Tok::Token &;
auto next_t(Parser &p) -> OptToken;
auto expect(Parser &p, const Tok::Id id) -> OptToken;

auto parse(Parser &p) -> void;
auto parse_toplevel(Parser &p) -> Ast::Node *;
auto parse_expr(Parser &p) -> Ast::Node *;
auto parse_primary(Parser &p) -> Ast::Node *;
auto parse_paren(Parser &p) -> Ast::Node *;
auto parse_decl(Parser &p) -> Ast::Node *;

auto parse_scope_statement(Parser &p) -> Ast::Node *;
auto parse_scope_type(Parser &p) -> Ast::Node *;

auto parse_fn_proto(Parser &p) -> Ast::Node *;
auto parse_fn_body(Parser &p) -> Ast::Node *;

auto parse_param(Parser &p) -> Ast::Node *;

template <typename Fn>
  requires std::invocable<Fn, Parser &>
auto parse_bin(Parser &p, const Fn &fn, auto... ops) -> Ast::Node * {
  auto lhs = std::invoke(fn, p);
  for (;;) {
    auto id = curr_t(p).id;

    auto flag = false;
    ((flag ^= (id == ops)), ...);
    if (!flag) {
      return lhs;
    }

    next_t(p);
    lhs = Ast::gen_node(Ast::NodeBinExpr{
        .tok = &curr_t(p), .op = id, .lhs = lhs, .rhs = std::invoke(fn, p)});
  }

  return lhs;
}

inline auto parse_bin_multiplicative(Parser &p) -> Ast::Node * {
  return parse_bin(p, parse_primary, Tok::ForwardSlash, Tok::Asterix);
}

inline auto parse_bin_addative(Parser &p) -> Ast::Node * {
  return parse_bin(p, parse_bin_multiplicative, Tok::Plus, Tok::Dash);
}

auto parse_type(Parser &p) -> Ast::Node *;
auto parse_lit(Parser &p) -> Ast::Node *;
auto parse_ident(Parser &p) -> Ast::Node *;

template <typename N, typename T>
inline auto gen_lit(Tok::Token &tok, T parsed) -> Ast::Node * {
  return Ast::gen_node(N{.tok = &tok, .val = parsed});
}

inline auto parse_lit_int(Tok::Token &tok) -> Ast::Node * {
  return gen_lit<Ast::NodeLitInt>(
      tok, std::stoll(std::string{tok.str.ptr, tok.str.cap}));
}

inline auto parse_lit_dec(Tok::Token &tok) -> Ast::Node * {
  return gen_lit<Ast::NodeLitDec>(
      tok, std::stold(std::string{tok.str.ptr, tok.str.cap}));
}

inline auto parse_lit_chr(Tok::Token &tok) -> Ast::Node * {
  return gen_lit<Ast::NodeLitChr>(tok, *ptr::add(tok.str.ptr, 1));
}

inline auto parse_lit_str(Tok::Token &tok) -> Ast::Node * {
  return gen_lit<Ast::NodeLitStr>(
      tok, Slice<const char>::from_unchecked(ptr::add(tok.str.ptr, 1),
                                             tok.str.cap - 2));
}

struct ParseError {
  using Self = ParseError;
  enum Kind {
    NotFound,
    Read,
    UnexpectedEof,
    Other,
  };

  Parser *p;
  Kind kind;

  [[nodiscard]] auto description() const -> std::string {
    auto msg = "unknown error";
    switch (kind) {
    default:
      break;
    case NotFound: {
      msg = "file not found";
      break;
    }
    case Read: {
      msg = "could not read file";
      break;
    }
    case UnexpectedEof: {
      msg = "unexpected EOF";
      break;
    }
    }

    return fmt::format("Parser Error in '{}' : ({},{}) - \"{}\"",
                       p->reader.fname, p->reader.line, p->reader.col, msg);
  }

  [[nodiscard]] static auto from(Parser *_p, Kind _kind) -> Self {
    return Self{.p = _p, .kind = _kind};
  }

  [[nodiscard]] static auto from(Parser *_p, Tokenizer::ReaderError err)
      -> Self {
    using Tokenizer::ReaderError;

    auto _kind = Other;
    switch (err.kind) {
    default:
      break;
    case ReaderError::NotFound: {
      _kind = NotFound;
      break;
    }
    case ReaderError::Read: {
      _kind = Read;
      break;
    }
    case ReaderError::UnexpectedEof: {
      _kind = UnexpectedEof;
      break;
    }
    }

    auto self = Self{.p = _p, .kind = _kind};
    self.p->reader = *err.r;
    return self;
  }
};
} // namespace Parser
