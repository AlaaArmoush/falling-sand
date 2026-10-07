#include "simulation/Simulation.hpp"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <utility>

namespace sand {
Simulation::Simulation(int width, int height, std::uint32_t seed)
    : width_(width), height_(height), seed_(seed), randomEngine_(seed),
      appearanceDistribution_(
          0, static_cast<int>(SAND_APPEARANCE_SHADES_COUNT - 1)) {
  if (width <= 0 || height <= 0) {
    throw std::invalid_argument("Simulation dimensions must be positive");
  }

  const std::size_t cellWidth = static_cast<std::size_t>(width);
  const std::size_t cellHeight = static_cast<std::size_t>(height);

  if (cellWidth > cells_.max_size() / cellHeight) {
    throw std::length_error("Simulation grid is too large");
  }

  cells_.resize(cellWidth * cellHeight); // our grid
}

int Simulation::width() const { return width_; }
int Simulation::height() const { return height_; }
std::uint32_t Simulation::seed() const { return seed_; }

std::optional<Cell> Simulation::cellAt(int x, int y) const {
  if (!isInBounds(x, y)) {
    return std::nullopt;
  }

  return cells_[indexOf(x, y)];
}

bool Simulation::isInBounds(int x, int y) const {
  return x >= 0 && x < width_ && y >= 0 && y < height_;
}

std::size_t Simulation::indexOf(int x, int y) const {
  return static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
         static_cast<std::size_t>(x);
}

bool Simulation::placeSand(int x, int y) {
  if (!isInBounds(x, y)) {
    return false;
  }

  Cell &cell = cells_[indexOf(x, y)];

  if (cell.occupied) {
    return false;
  }

  const auto appearance =
      static_cast<std::uint8_t>(appearanceDistribution_(randomEngine_));
  cell = Cell{true, appearance};

  return true;
}

void Simulation::step() {
  // Start one row above the bottom because the bottom row cannot fall.
  for (int y = height_ - 2; y >= 0; --y) {
    for (int x = 0; x < width_; ++x) {
      Cell &current = cells_[indexOf(x, y)];
      Cell &below = cells_[indexOf(x, y + 1)];

      if (current.occupied && !below.occupied) {
        std::swap(current, below);
      }
    }
  }
}

void Simulation::clear() { std::fill(cells_.begin(), cells_.end(), Cell{}); }
} // namespace sand
