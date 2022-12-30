#pragma once

#include "tok.hpp"

namespace Ast {

struct Ctx {
  Tok::Token &tok;

  [[nodiscard]] constexpr explicit Ctx(Tok::Token &&_tok) : tok{_tok} {}
};

struct Ident;
struct LitInt;
struct LitDec;
struct ExprBin;

using Node = Union<Ident, ExprBin>;

struct Ident : Ctx {
  using Ctx::Ctx;
};

struct LitInt : Ctx {
  s32 val{};
};

struct LitDec : Ctx {
  using Ctx::Ctx;

  d64 val{};
};

struct ExprBin : Ctx {
  using Ctx::Ctx;

  Tok::Id op{};
  Node *lhs{};
  Node *rhs{};
};

} // namespace Ast