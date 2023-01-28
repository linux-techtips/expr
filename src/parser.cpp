#include "include/parser.hpp"
#include "include/ast.hpp"

namespace Parser {

[[nodiscard]] auto Parser::from(const char *path) -> Result<Self, ParseError> {
  auto parser = Self{
      .file_path = path,
      .file_name = path, // TODO (Carter) change to actual file path and name
  };

  auto reader_res = Tokenizer::Reader::from(path);
  if (reader_res.is_err()) {
    return {ParseError::from(&parser, reader_res.err().unwrap())};
  }

  parser.reader = reader_res.ok().unwrap();
  Tokenizer::tokenize(parser.reader);

  return {parser};
}

auto curr_t(Parser &p) -> Tok::Token & { return p.reader.toks[p.pos]; }

auto next_t(Parser &p) -> OptToken {
  if (p.pos >= p.reader.size) {
    return {};
  }
  ++p.pos;
  return {curr_t(p)};
}

auto expect(Parser &p, const Tok::Id id) -> OptToken {
  if (p.pos >= p.reader.size) {
    return {};
  }

  ++p.pos;
  auto tok = curr_t(p);

  if (tok.id != id) {
    return {};
  }
  return {tok};
}

auto parse_toplevel(Parser &p) -> Ast::Node * {
  switch (curr_t(p).id) {
  default: {
    eprintln("Invalid parser state in toplevel expression");
    std::exit(1);
  }

  case Tok::Ident:
    return parse_decl(p);
  case Tok::End:
    return nullptr;
  }
}

auto parse_expr(Parser &p) -> Ast::Node * {
  Ast::Node *expr = {};

  switch (curr_t(p).id) {
  default: {
    expr = parse_bin_addative(p);
  }

  case Tok::OpenBracket: {
    expr = parse_fn_body(p);
  }
  }

  return expr;
}

auto parse_primary(Parser &p) -> Ast::Node * {
  switch (curr_t(p).id) {
  default: {
    eprintln("Invalid parser state in primary expression: {}", curr_t(p).id);
    std::exit(1);
  }

  case Tok::OpenParen: {
    return parse_paren(p);
  }
  case Tok::Ident: {
    return parse_ident(p);
  }
  case LIT_CASE: {
    return parse_lit(p);
  }
  }
}

auto parse_paren(Parser &p) -> Ast::Node * {
  return match(expect(p, Tok::CloseParen))(
      [](Tok::Token &t) -> Ast::Node * {
        eprintln("Expected expression in parentheses");
        std::exit(1);
        return nullptr;
      },
      [&p](None _) {
        Ast::Node *expr = parse_expr(p);
        if (curr_t(p).id != Tok::CloseParen) {
          eprintln("Unmatched closing parentheses");
          std::exit(1);
        }
        next_t(p); // can fail
        return expr;
      });

  return nullptr;
}

auto parse_decl(Parser &p) -> Ast::Node * {
  auto decl = Ast::NodeDecl{};

  decl.ident = parse_ident(p);

  auto id = curr_t(p).id;
  if (id == Tok::ColonColon || id == Tok::ColonEq) {
    eprintln("Type inference not implemented yet");
    std::exit(1);
  }

  if (id != Tok::Colon) {
    eprintln("Expected colon in declaration");
    std::exit(1);
  }

  next_t(p);
  decl.type = parse_type(p);

  id = curr_t(p).id;
  if (id == Tok::Colon) {
    decl.mut = Ast::Const;

  } else if (id == Tok::Eq) {
    decl.mut = Ast::Mut;

  } else {
    eprintln("Expected : or = in declaration");
    std::exit(1);
  }

  next_t(p);

  decl.expr = parse_expr(p);

  return Ast::gen_node(decl);
}

auto parse_fn_proto(Parser &p) -> Ast::Node * {
  auto proto = Ast::NodeFnProto{
      .tok = &curr_t(p),
      .params = {},
  };

  next_t(p); // can fail

  if (curr_t(p).id != Tok::OpenParen) {
    eprintln("Expected function declaration");
    std::exit(1);
  }
  next_t(p); // can fail

  for (; curr_t(p).id != Tok::CloseParen;) {
    proto.params.push_back(parse_param(p));
    if (curr_t(p).id == Tok::Comma) {
      next_t(p); // can fail
    }
  }

  next_t(p); // can fail
  if (curr_t(p).id == Tok::RightArrow) {
    next_t(p); // can fail
    proto.ret_type = parse_type(p);
  }

  return Ast::gen_node(proto);
}

auto parse_fn_body(Parser &p) -> Ast::Node * {
  eprintln("Unimplemented");
  std::exit(1);

  auto body = Ast::NodeFnBody{.tok = &curr_t(p)};
  next_t(p);

  if (curr_t(p).id != Tok::End) {
    next_t(p);
  }

  for (; curr_t(p).id != Tok::CloseBracket;) {
    body.statements.push_back(parse_expr(p));
    next_t(p);
  }

  return gen_node(body);
}

auto parse_param(Parser &p) -> Ast::Node * {
  auto ident = parse_ident(p);
  if (curr_t(p).id != Tok::Colon) {
    eprintln("Expected colon in function parameter");
    std::exit(1);
  }

  next_t(p); // can fail

  return Ast::gen_node(Ast::NodeParamDecl{
      .ident = ident,
      .type = parse_type(p),
  });
}

auto parse_type(Parser &p) -> Ast::Node * {

  switch (curr_t(p).id) {
  default: {
    return parse_ident(p);
  }
  case Tok::Fn: {
    return parse_fn_proto(p);
  }
  }
}

auto parse_lit(Parser &p) -> Ast::Node * {
  Ast::Node *res{};
  auto curr = curr_t(p);
  switch (curr.id) {
  default:
    eprintln("Expected literal, got: {} {}", curr.id, Tok::LitStr);
    std::exit(1);
  case Tok::LitInt: {
    res = parse_lit_int(curr);
    break;
  }
  case Tok::LitDec: {
    res = parse_lit_dec(curr);
    break;
  }
  case Tok::LitChr: {
    res = parse_lit_chr(curr);
    break;
  }
  case Tok::LitStr: {
    res = parse_lit_str(curr);
    break;
  }
  }

  next_t(p); // can fail
  return res;
}

auto parse_ident(Parser &p) -> Ast::Node * {
  auto t = curr_t(p);
  next_t(p); // can fail
  return Ast::gen_node(Ast::NodeIdent{
      .tok = &t,
      .name = t.str,
  });
}

} // namespace Parser
