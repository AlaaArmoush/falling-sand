#include "simulation/Simulation.hpp"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <random>
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

void Simulation::placeSandBrush(int centerX, int centerY, int radius,
                                double placementProbability) {
  constexpr int MAX_BRUSH_RADIUS = 64;

  if (radius < 0 || radius > MAX_BRUSH_RADIUS) {
    throw std::invalid_argument("Brush radius must be between 0 and 64");
  }

  if (placementProbability < 0.0 || placementProbability > 1.0) {
    throw std::invalid_argument(
        "Placement probability must be between 0 and 1");
  }

  if (placementProbability == 0.0) {
    return;
  }

  const int squaredRadius = radius * radius;

  // two outcomes either place grain or skip
  std::bernoulli_distribution placementTrial(placementProbability);

  for (int offsetY = -radius; offsetY <= radius; ++offsetY) {
    for (int offsetX = -radius; offsetX <= radius; ++offsetX) {
      const int squaredDistance = offsetX * offsetX + offsetY * offsetY;

      if (squaredDistance > squaredRadius) {
        continue;
      }

      const int candidateX = centerX + offsetX;
      const int candidateY = centerY + offsetY;

      if (!isInBounds(candidateX, candidateY)) {
        continue;
      }

      Cell &cell = cells_[indexOf(candidateX, candidateY)];

      if (cell.material != Material::Empty) {
        continue;
      }

      if (!placementTrial(randomEngine_)) {
        continue;
      }

      const auto appearance =
          static_cast<std::uint8_t>(appearanceDistribution_(randomEngine_));
      cell = Cell{Material::Sand, appearance};
    }
  }
}

void Simulation::step() {
  // Start one row above the bottom because the bottom row cannot fall.
  for (int y = height_ - 2; y >= 0; --y) {
    for (int x = 0; x < width_; ++x) {
      Cell &current = cells_[indexOf(x, y)];
      if (current.material == Material::Empty) {
        continue;
      }

      // Bottom-up: try down, then down-left, then down-right.
      const int destinationY = y + 1;

      // Bottom-up: try down, then down-left, then down-right.
      if (isInBounds(x, destinationY)) {
        Cell &below = cells_[indexOf(x, destinationY)];

        if (below.material == Material::Empty) {
          std::swap(current, below);
          continue;
        }
      }

      const int leftX = x - 1;
      if (isInBounds(leftX, destinationY)) {
        Cell &downLeft = cells_[indexOf(leftX, destinationY)];

        if (downLeft.material == Material::Empty) {
          std::swap(current, downLeft);
          continue;
        }
      }

      const int rightX = x + 1;
      if (isInBounds(rightX, destinationY)) {
        Cell &downRight = cells_[indexOf(rightX, destinationY)];

        if (downRight.material == Material::Empty) {
          std::swap(current, downRight);
          continue;
        }
      }
    }
  }
}

void Simulation::clear() { std::fill(cells_.begin(), cells_.end(), Cell{}); }
} // namespace sand
