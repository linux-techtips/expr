#include "include/parser.hpp"

auto main() -> int {
  auto parser = match(Parser::Parser::from("test.expr"))(
      [](const Parser::Parser &p) { return p; },
      [](const Parser::ParseError &err) {
        eprintln("{}", err);
        std::exit(1);
        return Parser::Parser{};
      });

  Ast::dump(Parser::parse_toplevel(parser));
}
