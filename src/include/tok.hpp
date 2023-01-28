#pragma once

#include <exl/core.hpp>
using namespace exl; // NOLINT

namespace Tok {

static constexpr const usize KEYWORDS_LEN = 39;
static constexpr const char *KEYWORDS[KEYWORDS_LEN] = { // NOLINT
    "s8",     "u8",    "s16",      "u16",   "s32",   "u32",   "s64",  "u64",
    "ssize",  "usize", "d32",      "d64",   "char",  "bool",  "true", "false",
    "const",  "mut",   "struct",   "enum",  "trait", "impl",  "fn",   "module",
    "import", "pub",   "if",       "elif",  "else",  "match", "case", "for",
    "in",     "break", "continue", "defer", "ret",   "or",    "as"};

enum Id : u8 {
  S8,
  U8,
  S16,
  U16,
  S32,
  U32,
  S64,
  U64,
  SSize,
  USize,
  D32,
  D64,
  Char,
  Bool,
  True,
  False,
  Cont,
  Mut,
  Struct,
  Enum,
  Trait,
  Impl,
  Fn,
  Module,
  Import,
  Pub,
  If,
  Elif,
  Else,
  Match,
  Case,
  For,
  In,
  Break,
  Continue,
  Defer,
  Ret,
  Or,
  As,

  End,

  Ident,
  LitInt,
  LitDec,
  LitStr,
  LitChr,

  SemiColon, // ;
  QstM,      // ?
  Dot,       // .
  Comma,     // ,

  DQuote, // "
  SQuote, // '

  OpenBracket,  // {
  CloseBracket, // }
  OpenParen,    // (
  CloseParen,   // )
  OpenArr,      // [
  CloseArr,     // ]

  Plus,         // +
  Dash,         // -
  Asterix,      // *
  ForwardSlash, // /
  Percent,      // %
  ExlM,         // !
  Carrot,       // ^
  Pipe,         // |
  And,          // &

  GrThan, // >
  LsThan, // <

  Eq,    // =
  Colon, // :

  SemiColonSemiColon, // ;;
  DotDot,             // ..

  PlusPlus,                 // ++
  DashDash,                 // --
  AsterixAsterix,           // **
  ForwardSlashForwardSlash, // //

  ColonColon,     // ::
  ColonEq,        // :=
  PlusEq,         // +=
  DashEq,         // -=
  AsterixEq,      // *=
  ForwardSlashEq, // /=
  CarrotEq,       // ^=
  PipeEq,         // |=
  AndEq,          // &=
  PercentEq,      // %=

  EqEq,     // ==
  ExlMEq,   // !=
  GrThanEq, // >=
  LsThanEq, // <=
  AndAnd,   // &&
  PipePipe, // ||

  LeftArrow,  // <-
  RightArrow, // ->
};

struct Token {
  using Self = Token;

  Id id{};
  usize start{};
  usize end{};
  Slice<const char> str{};

  [[nodiscard]] constexpr auto from(const Id _id, const usize _start,
                                    const usize _end, const char *_ptr,
                                    const usize _cap) {
    return Self(_id, _start, _end, _ptr, _cap);
  }

  [[nodiscard]] constexpr explicit Token() = default;

  [[nodiscard]] constexpr explicit Token(const Id _id, const usize _start,
                                         const usize _end, const char *_ptr,
                                         const usize _cap)
      : id{_id}, start{_start}, end{_end},
        str{Slice<const char>::from_unchecked(_ptr, _cap)} {}
};

constexpr auto change_id_if_keyword(Token &t) -> void {
  for (u8 i : Range(KEYWORDS_LEN - 1)) {
    if (std::memcmp(KEYWORDS[i], t.str.ptr, t.str.cap) == 0) { // NOLINT
      t.id = static_cast<Id>(i);
      return;
    }
  }
}

} // namespace Tok

template <> struct fmt::formatter<Tok::Token> {
  auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Tok::Token &t, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(
        ctx.out(), "{} [\n  start: {}\n  end: {}\n  str: {}\n]", t.id, t.start,
        t.end, std::string_view{t.str.ptr, t.str.cap});
  }
};
