#include <lexer/literals/numeric.hpp>

#include <iostream>

#include <charconv>

namespace expr::lexer::literals {

auto Numeric::parse(const std::string_view source) -> std::optional<Numeric> {
  auto value = decltype(Numeric::value) {};

  auto [ptr, ec] = std::from_chars(source.data(), source.data() + source.size(), value);
  if (ec != std::errc {}) {
    return {};
  }

  const auto len = std::distance(source.data(), ptr);
  return Numeric { source.substr(0, len), value };  
}

} // namespace expr::lexer::literals
