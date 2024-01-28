#include <lexer/lex.hpp>
#include <utils/assert.hpp>

namespace tests {

using namespace expr;

auto kind() -> void {
  using lexer::Kind;

  runtime_assert(Kind::Add().is_symbol(), "Add Kind was not symbol");
  runtime_assert(Kind::OpenParen().is_group(), "OpenParen kind was not group");

  runtime_assert(Kind::Sub().sizing() == 1, "Sub sizing was not 1");
  runtime_assert(Kind::Real().sizing() == 0, "Literals cannot have a sizing");
}

auto literal_numeric() -> void {
  using lexer::literals::Numeric;

  runtime_assert(Numeric::parse("1234.")->value == 1234.0, "Failed to parse numeric literal.");
  runtime_assert(Numeric::parse("1234")->text.size() == 4, "Incorrect text size.");
  runtime_assert(!Numeric::parse("abc").has_value(), "Invalid numeric literal parsed.");
}

} // namespace tests

auto main(int argc, char **argv) -> int {
  tests::kind();
  tests::literal_numeric();

  auto buffer = expr::lexer::lex("(123 * 123)");
  for (const auto tok : buffer.tokens) {
    std::cout << tok.fmt() << '\n';
  }
}
