#include "simulation/Simulation.hpp"

#include <algorithm>
#include <optional>
#include <stdexcept>

namespace sand {
Simulation::Simulation(int width, int height) : width_(width), height_(height) {
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

  cell = Cell{true, 0};
  return true;
}

void Simulation::clear() { std::fill(cells_.begin(), cells_.end(), Cell{}); }
} // namespace sand
