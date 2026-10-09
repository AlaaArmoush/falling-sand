#include "platform/RaylibView.hpp"
#include "raylib.h"
#include "simulation/Simulation.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace sand {

namespace {

constexpr std::array<Color, SAND_APPEARANCE_SHADES_COUNT> SAND_PALETTE{{
    Color{214, 174, 91, 255},
    Color{222, 185, 104, 255},
    Color{224, 189, 111, 255},
    Color{232, 196, 113, 255},
    Color{240, 205, 126, 255},
}};

// float because origin in screen space
constexpr float GRID_ORIGIN_X = 0.0F;
constexpr float GRID_ORIGIN_Y = 0.0F;
} // namespace
RaylibView::RaylibView(const Simulation &simulation, int scale)
    : gridWidth_(simulation.width()), gridHeight_(simulation.height()),
      scale_(scale), pixels_(static_cast<std::size_t>(gridWidth_) *
                                 static_cast<std::size_t>(gridHeight_),
                             Color{24, 24, 28, 255}),
      texture_{} {
  // raw image supplied to GPU from CPU to create texture
  Image image = GenImageColor(gridWidth_, gridHeight_, Color{24, 24, 28, 255});
  if (image.data == nullptr) {
    throw std::runtime_error("failed to create grid image");
  }

  texture_ = LoadTextureFromImage(image);
  // free the image data from CPU's ram
  UnloadImage(image);

  if (texture_.id == 0) {
    throw std::runtime_error("failed to create texture");
  }
  // nearest-neighbor filtering for sharp cell edges when enlarged
  SetTextureFilter(texture_, TEXTURE_FILTER_POINT);
}

RaylibView::~RaylibView() { UnloadTexture(texture_); }

InputCommands RaylibView::pollInput() const {
  InputCommands commands;

  commands.clearRequested = IsKeyPressed(KEY_C);
  commands.pauseToggleRequested = IsKeyPressed(KEY_SPACE);
  commands.singleStepRequested = IsKeyPressed(KEY_N);

  if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    return commands;
  }

  const Vector2 mousePosition = GetMousePosition();
  const float displayWidth = static_cast<float>(gridWidth_ * scale_);
  const float displayHeight = static_cast<float>(gridHeight_ * scale_);

  // reject outside positions
  if (mousePosition.x < GRID_ORIGIN_X ||
      mousePosition.x >= GRID_ORIGIN_X + displayWidth ||
      mousePosition.y < GRID_ORIGIN_Y ||
      mousePosition.y >= GRID_ORIGIN_Y + displayHeight) {
    return commands;
  }

  const int gridX = static_cast<int>(mousePosition.x - GRID_ORIGIN_X) / scale_;
  const int gridY = static_cast<int>(mousePosition.y - GRID_ORIGIN_Y) / scale_;

  commands.paintPosition = GridPosition{gridX, gridY};
  return commands;
}

void RaylibView::draw(const Simulation &simulation) {
  for (int y = 0; y < gridHeight_; ++y) {
    for (int x = 0; x < gridWidth_; ++x) {
      const Cell cell = simulation.cellAt(x, y).value();

      // index = y*width + x
      const std::size_t pixelIndex =
          static_cast<std::size_t>(y) * static_cast<std::size_t>(gridWidth_) +
          static_cast<std::size_t>(x);

      pixels_[pixelIndex] = colorForCell(cell);
    }
  }
  UpdateTexture(texture_, pixels_.data());
  // note: WHITE here means no change in color
  DrawTextureEx(texture_, Vector2{GRID_ORIGIN_X, GRID_ORIGIN_Y}, 0.0F, scale_,
                WHITE);
}

Color RaylibView::colorForCell(const Cell &cell) const {
  switch (cell.material) {
  case Material::Empty:
    return Color{24, 24, 28, 255};

  case Material::Sand:
    return SAND_PALETTE[cell.appearance];
  }

  throw std::runtime_error("unknown material identity");
}

} // namespace sand
