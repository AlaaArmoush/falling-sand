#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <sys/types.h>
#include <vector>

namespace sand {

constexpr std::uint8_t SAND_APPEARANCE_SHADES_COUNT = 5;

struct Cell {
  bool occupied = false;
  std::uint8_t appearance = 0; // cell color shade
};

class Simulation {
public:
  Simulation(int width, int height, std::uint32_t seed = 1);

  std::uint32_t seed() const;
  int width() const;
  int height() const;
  // nullopt means out-of-bounds/ Cell means in-bounds
  std::optional<Cell> cellAt(int x, int y) const;

  void placeSandBrush(int centerX, int centerY, int radius);

  void step();

  void clear();

private:
  bool isInBounds(int x, int y) const;
  std::size_t indexOf(int x, int y) const;

  int width_;
  int height_;
  uint32_t seed_;
  std::vector<Cell> cells_;

  // engine seeded once
  std::mt19937 randomEngine_;
  std::uniform_int_distribution<int> appearanceDistribution_;
};

} // namespace sand
