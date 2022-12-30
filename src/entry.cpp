#include "include/parser.hpp"

auto main() -> int {
  auto reader = match(Tokenizer::Reader::from("test.expr"))(
      [](const Tokenizer::Reader &r) { return r; },
      [](const std::string &str) {
        eprintln("{}", str);
        std::exit(1);
        return Tokenizer::Reader{};
      });

  Tokenizer::tokenize(reader);

  auto parser = match(Parser::Parser::from(reader))(
      [](const Parser::Parser &p) { return p; },
      [](const std::string &str) {
        eprintln("{}", str);
        std::exit(1);
        return Parser::Parser{};
      });

  auto _ = defer([&parser]() { delete[] parser.contents; }); // NOLINT

  /* auto node = Parser::parse_ident(parser);
  std::visit([](auto &&arg) { println("{}", arg.tok); },
             std::forward<decltype(*node)>(*node)); */
}