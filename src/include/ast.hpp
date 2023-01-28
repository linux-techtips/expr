#pragma once

#include "fmt/core.h"
#include "tokenizer.hpp"

namespace Ast {

struct NodeRoot;

struct NodeDecl;

struct NodeFnProto;
struct NodeFnBody;

struct NodeParamDecl;

struct NodeBinExpr;

struct NodeLitInt;
struct NodeLitDec;
struct NodeLitChr;
struct NodeLitStr;

struct NodeIdent;

using Node = Union<NodeRoot, NodeDecl, NodeFnProto, NodeFnBody, NodeParamDecl,
                   NodeBinExpr, NodeLitInt, NodeLitDec, NodeLitChr,
                   NodeLitStr, NodeIdent>;

struct NodeRoot {
  Tok::Token *tok{};
  std::vector<Node *> toplevel{};
};

enum Mutability : u8 {
  Mut,
  Const,
};

struct NodeDecl {
  Mutability mut{};
  Tok::Token *tok{};
  Node* ident{};
  Node* type{};
  Node* expr{}; // wow, it's like the name of the language or something
};

struct NodeFnProto {
  Tok::Token *tok{};
  Node *ret_type{};
  std::vector<Node *> params{};
};

struct NodeFnBody {
  Tok::Token *tok{};
  std::vector<Node *> statements{};
};

struct NodeParamDecl {
  Tok::Token *tok{};
  Node *ident{};
  Node *type{};
};

struct NodeBinExpr {
  Tok::Token *tok{};
  Tok::Id op{};
  Node *lhs{};
  Node *rhs{};
};

struct NodeLitInt {
  Tok::Token *tok{};
  ssize val{};
};

struct NodeLitDec {
  Tok::Token *tok{};
  d128 val{};
};

struct NodeLitChr {
  Tok::Token *tok{};
  char val{};
};

struct NodeLitStr {
  Tok::Token *tok{};
  Slice<const char> val{};
};

struct NodeIdent {
  Tok::Token *tok{};
  Slice<const char> name{};
};

template <typename N> static auto gen_node(const N &node) -> Node * {
  return new Node{node}; // NOLINT
}

template <typename N>
concept IsNode = requires(N n) {
                   { n.tok };
                 };

struct Depth {
  using Self = Depth;
  usize val{};

  [[nodiscard]] static constexpr auto increase(const Depth depth,
                                               const usize n = 2) -> Depth {
    return Depth{.val = depth.val + n};
  };
};

auto dump(const Node *node, const Depth = Depth{.val = 0}) -> void;

} // namespace Ast

template <> struct fmt::formatter<Ast::Depth> {
  constexpr auto parse(fmt::format_parse_context &ctx)
      -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Ast::Depth depth, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", std::string(depth.val, ' '));
  }
};
