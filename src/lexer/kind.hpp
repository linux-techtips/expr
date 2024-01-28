#pragma once

#include <string_view>

#include <utils/types.hpp>

namespace {

template <size_t N>
constexpr auto len(char const (&)[N]) -> u8 {
  return N - 1;
}

} // namespace private

namespace expr::lexer {

struct Kind { 
  u8 inner;

  #define TOKEN(Name) \
    [[nodiscard, gnu::always_inline]] \
    constexpr static auto Name() -> Kind { return { __COUNTER__ }; }
  #include <lexer/kind.def>
  #undef TOKEN

  struct Tables {
    constexpr static auto size = __COUNTER__;

    const static bool literals[size];
    const static bool symbols[size];
    const static bool groupings[size];
    const static u8 sizings[size];
    const static std::string_view namings[size];
  };

  [[nodiscard]]
  auto fmt() const -> std::string_view {
    return Tables::namings[inner]; 
  }

  [[nodiscard, gnu::always_inline]]
  constexpr auto is_literal() const -> bool {
    return Tables::literals[inner];
  }

  [[nodiscard, gnu::always_inline]]
  constexpr auto is_symbol() const -> bool {
    return Tables::symbols[inner];
  }

  [[nodiscard, gnu::always_inline]]
  constexpr auto is_group() const -> bool {
    return Tables::groupings[inner];
  }

  [[nodiscard, gnu::always_inline]]
  constexpr auto sizing() const -> u8 {
    return Tables::sizings[inner];
  }
};

constexpr bool Kind::Tables::literals[] = {
  #define TOKEN(Name) false,
  #define TOKEN_LITERAL(Name) true,
  #include <lexer/kind.def>
  #undef TOKEN
  #undef TOKEN_LITERAL
};

constexpr bool Kind::Tables::symbols[] = {
  #define TOKEN(Name) false,
  #define TOKEN_SYMBOL(Name, Spelling) true,
  #include <lexer/kind.def>
  #undef TOKEN
  #undef TOKEN_SYMBOL
};

constexpr bool Kind::Tables::groupings[] = {
  #define TOKEN(Name) false,
  #define TOKEN_GROUP(Name, Spelling, Alt) true,
  #include <lexer/kind.def>
  #undef TOKEN
  #undef TOKEN_GROUP
};

constexpr u8 Kind::Tables::sizings[] = {
  #define TOKEN(Name) 0,
  #define TOKEN_SIZED(Name, Size) Size,
  #define TOKEN_SYMBOL(Name, Spelling) len(Spelling),
  #include <lexer/kind.def>
  #undef TOKEN
  #undef TOKEN_SIZED
  #undef TOKEN_SYMBOL
};

constexpr std::string_view Kind::Tables::namings[] = {
  #define TOKEN(Name) #Name,
  #include <lexer/kind.def>
  #undef TOKEN
};

} // namespace expr::lexer
