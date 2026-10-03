#include "platform/RaylibView.hpp"

#include "raylib.h"
#include "simulation/Simulation.hpp"

#include <cstddef>
#include <stdexcept>

namespace sand {
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
  DrawTextureEx(texture_, Vector2{0.0F, 0.0F}, 0.0F, static_cast<float>(scale_),
                WHITE);
}

Color RaylibView::colorForCell(const Cell &cell) const {
  if (cell.occupied) {
    return BEIGE;
  }

  return Color{24, 24, 28, 255};
}

} // namespace sand
