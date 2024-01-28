#include "literals/numeric.hpp"
#include <array>

#include <lexer/lex.hpp>
#include <lexer/pred.hpp>

namespace expr::lexer {

struct Lexer {
  Buffer buffer;
  Location loc;

  [[nodiscard]]
  auto lex() && -> Buffer;

  [[nodiscard, gnu::always_inline]]
  auto curr() -> char;

  auto inc(const u32 amt = 1) -> void;

  auto lex_invalid() -> void;
  auto lex_horizontal_whitespace() -> void;
  auto lex_vertical_whitespace() -> void;

  auto lex_symbol_add() -> void;
  auto lex_symbol_sub() -> void;
  auto lex_symbol_mul() -> void;
  auto lex_symbol_div() -> void;
  auto lex_open_paren() -> void;
  auto lex_close_paren() -> void;
  auto lex_numeric_literal() -> void;
};

struct Dispatcher {
  using StaticFn = auto (*)(Lexer&) -> void;
  using MemberFn = auto (Lexer::*)() -> void;

  using Table = std::array<StaticFn, 256>;
  const static Table table; 

  static auto chain(Lexer& lexer) -> void {
    const auto idx = static_cast<usize>(lexer.loc.idx);
    const auto src = lexer.buffer.source;

    [[likely]] if (idx < src.size()) {
      [[clang::musttail]] return table[src[idx]](lexer);
    }

    lexer.buffer.push_token({ Kind::EndOfFile(), lexer.loc });
  } 

  template <MemberFn Fn>
  static auto wrap(Lexer& lexer) -> void {
    (lexer.*Fn)();
    [[clang::musttail]] return chain(lexer);
  };
};

constexpr Dispatcher::Table Dispatcher::table = [] constexpr {
  auto table = Dispatcher::Table {};

  table.fill(Dispatcher::wrap<&Lexer::lex_invalid>);

  table[' '] = &Dispatcher::wrap<&Lexer::lex_horizontal_whitespace>;
  table['\t'] = &Dispatcher::wrap<&Lexer::lex_horizontal_whitespace>;
  table['\n'] = &Dispatcher::wrap<&Lexer::lex_vertical_whitespace>;

  table['+'] = Dispatcher::wrap<&Lexer::lex_symbol_add>;
  table['-'] = Dispatcher::wrap<&Lexer::lex_symbol_sub>;
  table['*'] = Dispatcher::wrap<&Lexer::lex_symbol_mul>;
  table['/'] = Dispatcher::wrap<&Lexer::lex_symbol_div>;

  table['('] = Dispatcher::wrap<&Lexer::lex_open_paren>;
  table[')'] = Dispatcher::wrap<&Lexer::lex_close_paren>;

  for (char c = '0'; c < '9'; ++c) {
    table[c] = Dispatcher::wrap<&Lexer::lex_numeric_literal>;
  }

  return table;
}();

auto lex(const std::string_view source) -> Buffer {
  return Lexer { .buffer = Buffer::from_source(source) }.lex();
}

auto Lexer::lex() && -> Buffer {
  Dispatcher::chain(*this);
  return buffer;
}

auto Lexer::inc(const u32 amt) -> void {
  for (u32 i = 0; i < amt && loc.idx < buffer.source.size(); ++i, ++loc.idx) {
    if (buffer.source[loc.idx] == '\n') {
      loc.line += 1;
      loc.column = 1;
    
    } else {
      loc.column += 1;
    }
  }
}

auto Lexer::curr() -> char {
  return buffer.source[loc.idx];
}

auto Lexer::lex_invalid() -> void {
  buffer.push_token({ Kind::Invalid(), loc });
  this->inc();
}

auto Lexer::lex_horizontal_whitespace() -> void {
  while (loc.idx < buffer.source.size() && pred::is_horizontal_whitespace(this->curr())) {
    this->inc();
  }
}

auto Lexer::lex_vertical_whitespace() -> void {
  while (loc.idx < buffer.source.size() && pred::is_vertical_whitespace(this->curr())) {
    this->inc();
  }
}

auto Lexer::lex_open_paren() -> void {
  const auto kind = Kind::OpenParen();
  buffer.push_token({ kind, loc });
  this->inc(kind.sizing());
}

auto Lexer::lex_close_paren() -> void {
  const auto kind = Kind::CloseParen();
  buffer.push_token({ kind, loc });
  this->inc(kind.sizing());
}

auto Lexer::lex_symbol_add() -> void {
  const auto kind = Kind::Add();
  buffer.push_token({ kind, loc });
  this->inc(kind.sizing());
}

auto Lexer::lex_symbol_sub() -> void {
  const auto kind = Kind::Sub();
  buffer.push_token({ kind, loc });
  this->inc(kind.sizing());
}

auto Lexer::lex_symbol_mul() -> void {
  const auto kind = Kind::Mul();
  buffer.push_token({ kind, loc });
  this->inc(kind.sizing());
}

auto Lexer::lex_symbol_div() -> void {
  const auto kind = Kind::Div();
  buffer.push_token({ kind, loc });
  this->inc(kind.sizing());
}

auto Lexer::lex_numeric_literal() -> void {
  const auto kind = Kind::Real();
  const auto source = buffer.source.substr(loc.idx);
  if (auto opt = literals::Numeric::parse(source)) {
    buffer.push_token({
        .kind = kind,
        .loc = {
          .line = loc.line,
          .column = loc.column,
          .idx = buffer.push_literal(*opt),
        },
    });

    this->inc(opt->text.size());

  } else {
    this->lex_invalid();
  }
}

} // namespace expr::lexer
