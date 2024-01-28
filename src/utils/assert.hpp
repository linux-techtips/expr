#pragma once

#include <source_location>
#include <string_view>
#include <iostream>
#include <format>

[[gnu::always_inline]]
constexpr auto assertion_msg(
  const std::string_view fmt,
  const std::string_view msg,
  const std::source_location loc = std::source_location::current()
) -> std::string {
  return std::format(
    "[{}]: \"{}\"\n"
    "  in {} at {}:{}\n"
    "  in {}\n", 
    fmt,
    msg,
    loc.file_name(),
    loc.line(),
    loc.column(),
    loc.function_name()
  ); 
}

[[gnu::always_inline]]
inline auto runtime_assert(
    bool cond,
    const std::string_view msg = "",
    const std::source_location loc = std::source_location::current()
) -> void {
  if (cond) return;

  std::cerr << assertion_msg("ASSERT", msg, loc);

  __builtin_trap();
}

[[noreturn, gnu::always_inline]]
inline auto runtime_todo(const std::source_location loc = std::source_location::current()) -> void {
  std::cerr << assertion_msg("TODO", "Aspects of this feature are not fully implemented", loc);
  __builtin_trap();
}
