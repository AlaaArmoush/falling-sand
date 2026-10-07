#pragma once

#include "raylib.h"
#include "simulation/Simulation.hpp"
#include <optional>
#include <vector>

namespace sand {

struct GridPosition {
  int x;
  int y;
};

struct InputCommands {
  bool clearRequested = false;
  std::optional<GridPosition> paintPosition;
  bool pauseToggleRequested = false;
  bool singleStepRequested = false;
};

class RaylibView {
public:
  RaylibView(const Simulation &simulation, int scale);

  // Non-copyable to ensure unique ownership of the hardware texture resource.
  RaylibView(const RaylibView &) = delete;
  RaylibView &operator=(const RaylibView &) = delete;

  ~RaylibView();

  InputCommands pollInput() const;

  void draw(const Simulation &simulation);

private:
  Color colorForCell(const Cell &cell) const;

  int gridWidth_;
  int gridHeight_;
  int scale_;
  std::vector<Color> pixels_;
  Texture2D texture_;
};

} // namespace sand
