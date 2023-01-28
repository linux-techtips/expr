#pragma once

#include "tok.hpp"

#include <concepts>
#include <vector>

namespace Tokenizer {

#define WHITESPACE_CASE                                                        \
  ' ' : case '\t':                                                             \
  case '\r'

#define NEWLINE_CASE '\n' // NOLINT

#define DIGIT_CASE                                                             \
  '0' : case '1':                                                              \
  case '2':                                                                    \
  case '3':                                                                    \
  case '4':                                                                    \
  case '5':                                                                    \
  case '6':                                                                    \
  case '7':                                                                    \
  case '8':                                                                    \
  case '9'

#define ALPHA_LOWER_CASE                                                       \
  'a' : case 'b':                                                              \
  case 'c':                                                                    \
  case 'd':                                                                    \
  case 'e':                                                                    \
  case 'f':                                                                    \
  case 'g':                                                                    \
  case 'h':                                                                    \
  case 'i':                                                                    \
  case 'j':                                                                    \
  case 'k':                                                                    \
  case 'l':                                                                    \
  case 'm':                                                                    \
  case 'n':                                                                    \
  case 'o':                                                                    \
  case 'p':                                                                    \
  case 'q':                                                                    \
  case 'r':                                                                    \
  case 's':                                                                    \
  case 't':                                                                    \
  case 'u':                                                                    \
  case 'v':                                                                    \
  case 'w':                                                                    \
  case 'x':                                                                    \
  case 'y':                                                                    \
  case 'z'

#define ALPHA_UPPER_CASE                                                       \
  'A' : case 'B':                                                              \
  case 'C':                                                                    \
  case 'D':                                                                    \
  case 'E':                                                                    \
  case 'F':                                                                    \
  case 'G':                                                                    \
  case 'H':                                                                    \
  case 'I':                                                                    \
  case 'J':                                                                    \
  case 'K':                                                                    \
  case 'L':                                                                    \
  case 'M':                                                                    \
  case 'N':                                                                    \
  case 'O':                                                                    \
  case 'P':                                                                    \
  case 'Q':                                                                    \
  case 'R':                                                                    \
  case 'S':                                                                    \
  case 'T':                                                                    \
  case 'U':                                                                    \
  case 'V':                                                                    \
  case 'W':                                                                    \
  case 'X':                                                                    \
  case 'Y':                                                                    \
  case 'Z'

#define ALPHA_CASE                                                             \
  ALPHA_LOWER_CASE:                                                            \
  case ALPHA_UPPER_CASE

#define ALNUM_LOWER_CASE                                                       \
  DIGIT_CASE:                                                                  \
  case ALPHA_LOWER_CASE

#define ALNUM_UPPER_CASE                                                       \
  DIGIT_CASE:                                                                  \
  case ALPHA_UPPER_CASE

#define IDENT_ALLOWED_CASE                                                     \
  '_' : case DIGIT_CASE:                                                       \
  case ALPHA_LOWER_CASE:                                                       \
  case ALPHA_UPPER_CASE

enum ReaderState : u8 {
  Start,
  Eof,

  SemiColon,
  Dot,
  Comma,

  Plus,
  Dash,
  Asterix,
  ForwardSlash,
  Percent,
  ExlM,
  Carrot,
  Pipe,
  And,

  GrThan,
  LsThan,

  Eq,
  Colon,

  Ident,
  LitNum,

  DQuote,
  SQuote,
};

struct ReaderError;

struct Reader {
  ReaderState state{Start};

  usize pos{0};
  usize line{0};
  usize col{1};

  usize size{0};
  const char *fname{};
  char *contents{};
  std::vector<Tok::Token> toks{};

  [[nodiscard]] static auto from(const char *_fname) noexcept
      -> Result<Reader, ReaderError>;
};

[[nodiscard]] auto inline curr_c(Reader &r) -> char & {
  return r.contents[r.pos]; // NOLINT
}

[[nodiscard]] auto inline peek_c(Reader &r, const usize offset)
    -> Option<std::reference_wrapper<char>> {
  auto idx = r.pos + offset;
  if (idx >= r.size) {
    return {};
  }
  return {r.contents[idx]}; // NOLINT
}

auto next_c(Reader &r) -> void;

[[nodiscard]] auto inline curr_t(Reader &r) -> Tok::Token & {
  return r.toks.back();
}

auto next_t(Reader &r) -> void;

auto inline gen_t(Reader &r, const Tok::Id id) -> void {
  r.toks.emplace_back(id, r.line, r.col, ptr::add(r.contents, r.pos), 1);
  next_c(r);
}

auto inline end_t(Reader &r, Tok::Token &t) -> void {
  t.str.cap = ptr::diff(ptr::add(r.contents, r.pos), t.str.ptr);
}

auto inline change_last_t(Reader &r, const Tok::Id id) -> void {
  next_c(r);
  auto &curr = curr_t(r);
  end_t(r, curr);
  curr.id = id;
}

auto inline change_state(Reader &r, const ReaderState s, const Tok::Id id)
    -> void {
  r.state = s;
  gen_t(r, id);
}

auto inline tokenize(Reader &r) -> void {
  for (; r.state != Tokenizer::Eof;) {
    next_t(r);
  }
  r.toks.emplace_back(Tok::Id::End, r.line, ++r.col,
                      ptr::add(r.contents, ++r.pos), 1);
}

struct IOError {
  using Self = IOError;
  enum Kind {
    NotFound,
    Read,
    Write,
    UnexpectedEof,
    Other,
  };

  Kind kind;

  [[nodiscard]] constexpr auto description() const -> const char * {
    switch (kind) {
    default:
      return "other error";
    case NotFound:
      return "entity not found";
    case Read:
      return "failed to read entity";
    case Write:
      return "failed to write to entity";
    case UnexpectedEof:
      return "unexpected EOF";
    }
  }

  [[nodiscard]] static constexpr auto from(Kind _kind) -> Self {
    return Self{.kind = _kind};
  }
};

struct ReaderError {
  using Self = ReaderError;
  enum Kind {
    NotFound,
    Read,
    UnexpectedEof,
    Other,
  };

  Reader *r;
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
      msg = "failed to read file";
      break;
    }
    case UnexpectedEof: {
      msg = "unexpected end of file";
      break;
    }
    }

    return fmt::format("Reader Error in {} : ({},{}) - \"{}\"", r->fname,
                       r->line, r->col, msg);
  }

  [[nodiscard]] static auto from(Reader *_r, Kind _kind) -> Self {
    return Self{.r = _r, .kind = _kind};
  }
};

} // namespace Tokenizer
