#include "simulation/Simulation.hpp"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <random>
#include <raylib.h>
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
  // Give either side a 50/50 chance to move first.
  std::bernoulli_distribution scanFromLeft(0.5);
  const bool leftToRight = scanFromLeft(randomEngine_);

  // Start one row above the bottom because the bottom row cannot fall.
  for (int y = height_ - 2; y >= 0; --y) {
    for (int offset = 0; offset < width_; ++offset) {
      const int x = leftToRight ? offset : width_ - 1 - offset;
      Cell &current = cells_[indexOf(x, y)];

      if (current.material == Material::Empty) {
        continue;
      }

      const int destinationY = y + 1;

      if (isInBounds(x, destinationY)) {
        Cell &below = cells_[indexOf(x, destinationY)];

        if (below.material == Material::Empty) {
          std::swap(current, below);
          continue;
        }
      }

      const int leftX = x - 1;
      const int rightX = x + 1;

      const bool leftIsEmpty =
          isInBounds(leftX, destinationY) &&
          cells_[indexOf(leftX, destinationY)].material == Material::Empty;

      const bool rightIsEmpty =
          isInBounds(rightX, destinationY) &&
          cells_[indexOf(rightX, destinationY)].material == Material::Empty;

      int destinationX = x;

      if (leftIsEmpty && rightIsEmpty) {
        std::bernoulli_distribution chooseLeft(0.5);
        destinationX = chooseLeft(randomEngine_) ? leftX : rightX;
      } else if (leftIsEmpty) {
        destinationX = leftX;
      } else if (rightIsEmpty) {
        destinationX = rightX;
      } else {
        continue;
      }

      std::swap(current, cells_[indexOf(destinationX, destinationY)]);
    }
  }
}

void Simulation::clear() { std::fill(cells_.begin(), cells_.end(), Cell{}); }
} // namespace sand
