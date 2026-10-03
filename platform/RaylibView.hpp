#pragma once

#include "raylib.h"
#include "simulation/Simulation.hpp"
#include <vector>

namespace sand {

class RaylibView {
public:
  RaylibView(const Simulation &simulation, int scale);

  // Non-copyable to ensure unique ownership of the hardware texture resource.
  RaylibView(const RaylibView &) = delete;
  RaylibView &operator=(const RaylibView &) = delete;

  ~RaylibView();

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
