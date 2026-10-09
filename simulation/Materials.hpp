#pragma once

#include <cstdint>

namespace sand {
enum class Material : std::uint8_t { Empty, Sand };

constexpr std::uint8_t SAND_APPEARANCE_SHADES_COUNT = 5;

struct Cell {
  Material material = Material::Empty;
  std::uint8_t appearance = 0;
};
} // namespace sand
