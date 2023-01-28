#include "include/tokenizer.hpp"

namespace Tokenizer {

auto next_c(Reader &r) -> void {
  if (++r.pos >= r.size - 1) {
    r.state = Eof;
    return;
  }

  switch (curr_c(r)) {
  default: {
    ++r.col;
    break;
  }
  case NEWLINE_CASE: {
    ++r.line;
    r.col = 0;
    break;
  }
  }
}

auto next_t(Reader &r) -> void {

  for (;;) {
    switch (r.state) {
    default: {
      eprintln("Unhandled Reader state {}", r.state);
      std::exit(1);
    }

    case Eof: {
      return;
    }

    case Start: {
      switch (curr_c(r)) {
      default: {
        eprintln("Unhandled Start state {}", curr_c(r));
        std::exit(1);
      }

      case ALPHA_CASE: {
        change_state(r, Ident, Tok::Id::Ident);
        continue;
      }

      case WHITESPACE_CASE: {
        next_c(r);
        continue;
      }

      case NEWLINE_CASE: {
        gen_t(r, Tok::Id::End);
        return;
      }

      case DIGIT_CASE: {
        change_state(r, LitNum, Tok::Id::LitInt);
        continue;
      }

      case ';': {
        change_state(r, SemiColon, Tok::Id::SemiColon);
        continue;
      }

      case '?': {
        gen_t(r, Tok::Id::QstM);
        return;
      }

      case '.': {
        change_state(r, Dot, Tok::Id::Dot);
        continue;
      }

      case ',': {
        gen_t(r, Tok::Id::Comma);
        return;
      }

      case '\'': {
        change_state(r, SQuote, Tok::Id::SQuote);
        continue;
      }

      case '"': {
        change_state(r, DQuote, Tok::Id::DQuote);
        continue;
      }

      case '{': {
        gen_t(r, Tok::Id::OpenBracket);
        return;
      }

      case '}': {
        gen_t(r, Tok::Id::CloseBracket);
        return;
      }

      case '(': {
        gen_t(r, Tok::Id::OpenParen);
        return;
      }

      case ')': {
        gen_t(r, Tok::Id::CloseParen);
        return;
      }

      case '[': {
        gen_t(r, Tok::Id::OpenArr);
        return;
      }

      case ']': {
        gen_t(r, Tok::Id::CloseArr);
        return;
      }

      case '+': {
        change_state(r, Plus, Tok::Id::Plus);
        continue;
      }

      case '-': {
        change_state(r, Dash, Tok::Id::Dash);
        continue;
      }

      case '*': {
        change_state(r, Asterix, Tok::Id::Asterix);
        continue;
      }

      case '/': {
        change_state(r, ForwardSlash, Tok::Id::ForwardSlash);
        continue;
      }

      case '%': {
        change_state(r, Percent, Tok::Id::Percent);
        continue;
      }

      case '!': {
        change_state(r, ExlM, Tok::Id::ExlM);
        continue;
      }

      case '^': {
        change_state(r, Carrot, Tok::Id::Carrot);
        continue;
      }

      case '|': {
        change_state(r, Pipe, Tok::Id::Pipe);
        continue;
      }

      case '&': {
        change_state(r, And, Tok::Id::And);
        continue;
      }

      case '>': {
        change_state(r, GrThan, Tok::Id::GrThan);
        continue;
      }

      case '<': {
        change_state(r, LsThan, Tok::Id::LsThan);
        continue;
      }

      case '=': {
        change_state(r, Eq, Tok::Id::Eq);
        continue;
      }

      case ':': {
        change_state(r, Colon, Tok::Id::Colon);
        continue;
      }
      }
    }

    case Ident: {
      auto _ = defer([&r]() {
        if (r.state != Eof) {
          r.state = Start;
        }
      });

      for (;;) {
        switch (curr_c(r)) {
        default: {
          auto &curr = curr_t(r);
          end_t(r, curr);
          Tok::change_id_if_keyword(curr);

          return;
        }

        case IDENT_ALLOWED_CASE: {
          next_c(r);
          continue;
        }
        }
      }
    }

    case LitNum: {
      auto _ = defer([&r]() {
        if (r.state != Eof) {
          r.state = Start;
        }
      });
      auto dec = false;

      for (;;) {
        switch (curr_c(r)) {
        default: {
          auto &curr = curr_t(r);
          end_t(r, curr);

          if (dec) {
            curr.id = Tok::Id::LitDec;
          }

          return; // NOLINT
        }

        case '.': {
          if (dec) {
            eprintln("Unexpected \".\"");
            std::exit(1);
          }
          dec = true;
        }

        case DIGIT_CASE: {
          next_c(r);
          continue;
        }
        }
      }
    }

    case SemiColon: {
      auto _ = defer([&r]() { r.state = Start; });

      switch (curr_c(r)) {
      default: {
        break;
      }

      case ';': {
        change_last_t(r, Tok::Id::SemiColonSemiColon);
        break;
      }
      }

      return;
    }

    case Dot: {
      auto _ = defer([&r]() { r.state = Start; });

      switch (curr_c(r)) {
      default: {
        break;
      }

      case '.': {
        change_last_t(r, Tok::Id::DotDot);
        break;
      }
      }

      return;
    }

    case Plus: {
      auto _ = defer([&r]() { r.state = Start; });

      switch (curr_c(r)) {
      default: {
        break;
      }

      case '=': {
        change_last_t(r, Tok::Id::PlusEq);
        break;
      }

      case '+': {
        change_last_t(r, Tok::Id::PlusPlus);
        break;
      }
      }

      return;
    }

    case Dash: {
      auto _ = defer([&r]() { r.state = Start; });

      switch (curr_c(r)) {
      default: {
        break;
      }

      case '=': {
        change_last_t(r, Tok::Id::DashEq);
        break;
      }

      case '-': {
        change_last_t(r, Tok::Id::DashDash);
        break;
      }

      case '>': {
        change_last_t(r, Tok::Id::RightArrow);
        break;
      }
      }

      return;
    }

    case Asterix: {
      auto _ = defer([&r]() { r.state = Start; });

      switch (curr_c(r)) {
      default: {
        break;
      }

      case '=': {
        change_last_t(r, Tok::Id::AsterixEq);
        break;
      }

      case '*': {
        change_last_t(r, Tok::Id::AsterixAsterix);
        break;
      }
      }

      return;
    }

    case ForwardSlash: {
      auto _ = defer([&r]() { r.state = Start; });
      switch (curr_c(r)) {
      default: {
        break;
      }

      case '=': {
        change_last_t(r, Tok::Id::ForwardSlashEq);
        break;
      }

      case '/': {
        change_last_t(r, Tok::Id::ForwardSlashForwardSlash);
        break;
      }
      }

      return;
    }

    case Carrot: {
      auto _ = defer([&r]() { r.state = Start; });
      switch (curr_c(r)) {
      default: {
        break;
      }

      case '=': {
        change_last_t(r, Tok::Id::CarrotEq);
        break;
      }
      }

      return;
    }

    case Pipe: {
      auto _ = defer([&r]() { r.state = Start; });
      switch (curr_c(r)) {
      default: {
        break;
      }

      case '=': {
        change_last_t(r, Tok::Id::PipeEq);
        break;
      }

      case '|': {
        change_last_t(r, Tok::Id::PipePipe);
        break;
      }
      }

      return;
    }

    case And: {
      auto _ = defer([&r]() { r.state = Start; });
      switch (curr_c(r)) {
      default: {
        break;
      }

      case '=': {
        change_last_t(r, Tok::Id::AndEq);
        break;
      }

      case '&': {
        change_last_t(r, Tok::Id::AndAnd);
        break;
      }
      }

      return;
    }

    case GrThan: {
      auto _ = defer([&r]() { r.state = Start; });
      switch (curr_c(r)) {
      default: {
        break;
      }

      case '=': {
        change_last_t(r, Tok::Id::GrThanEq);
        break;
      }
      }

      return;
    }

    case LsThan: {
      auto _ = defer([&r]() { r.state = Start; });
      switch (curr_c(r)) {
      default: {
        break;
      }

      case '=': {
        change_last_t(r, Tok::Id::LsThanEq);
        break;
      }

      case '-': {
        change_last_t(r, Tok::Id::LeftArrow);
        break;
      }
      }
      return;
    }

    case Eq: {
      auto _ = defer([&r]() { r.state = Start; });

      switch (curr_c(r)) {
      default: {
        break;
      }

      case '=': {
        next_c(r);
        auto &curr = curr_t(r);
        end_t(r, curr);
        curr.id = Tok::EqEq;

        break;
      }
      }

      return;
    }

    case Colon: {
      auto _ = defer([&r]() { r.state = Start; });

      switch (curr_c(r)) {
      default: {
        break;
      }

      case ':': {
        change_last_t(r, Tok::ColonColon);
        break;
      }

      case '=': {
        change_last_t(r, Tok::ColonEq);
        break;
      }
      }

      return;
    }

    case SQuote: {
      auto _ = defer([&r]() {
        if (r.state != Eof) {
          r.state = Start;
        }
      });

      if (curr_c(r) == '\'') {
        eprintln("Empty character literals cannot be defined");
        std::exit(1);
      }

      next_c(r);

      if (curr_c(r) != '\'') {
        eprintln("Expected '\''");
        std::exit(1);
      }

      next_c(r);

      change_last_t(r, Tok::LitChr);

      return; // NOLINT
    }

    case DQuote: {
      change_last_t(r, Tok::LitStr); // As of now, double quotes can only support string literals
      
      auto _ = defer([&r]() { r.state = Start; });
      for (; r.state != Eof;) {
        switch (curr_c(r)) {
        default: {
          next_c(r);
          continue;
        }

        case '"': {
          next_c(r);
          end_t(r, curr_t(r));
          return;
        }
        }
      }

      eprintln("Expected '\"'");
      std::exit(1);
    }
    }
  }
}

[[nodiscard]] auto Reader::from(const char *_fname) noexcept
    -> Result<Reader, ReaderError> {
  auto reader = Reader{.fname = _fname};

  auto fd = std::fopen(reader.fname, "r"); // NOLINT
  if (fd == nullptr) {
    return {ReaderError::from(&reader, ReaderError::NotFound)};
  }
  auto _ = exl::defer([&fd]() { std::fclose(fd); });

  std::fseek(fd, 0, SEEK_END);

  reader.size = static_cast<usize>(std::ftell(fd) + 1);

  reader.contents = new (std::nothrow) char[reader.size]; // NOLINT
  if (reader.contents == nullptr) {
    return {ReaderError::from(&reader, ReaderError::UnexpectedEof)};
  }

  std::fseek(fd, 0, SEEK_SET);

  std::fread(reader.contents, reader.size - 1, sizeof(char), fd);
  if (std::ferror(fd)) {
    return {ReaderError::from(&reader, ReaderError::Read)};
  }
  reader.contents[reader.size] = 0; // NOLINT
  return {reader};
}

} // namespace Tokenizer
