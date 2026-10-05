#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace sand {

struct Cell {
  bool occupied = false;
  std::uint8_t appearance = 0; // cell color shade
};

class Simulation {
public:
  Simulation(int width, int height);

  int width() const;
  int height() const;
  // nullopt means out-of-bounds/ Cell means in-bounds
  std::optional<Cell> cellAt(int x, int y) const;

  bool placeSand(int x, int y);

  void clear();

private:
  bool isInBounds(int x, int y) const;
  std::size_t indexOf(int x, int y) const;

  int width_;
  int height_;
  std::vector<Cell> cells_;
};

} // namespace sand
