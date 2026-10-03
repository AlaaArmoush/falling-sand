#include "platform/RaylibView.hpp"
#include "raylib.h"
#include "simulation/Simulation.hpp"

#include <exception>

int main() {
  constexpr int GRID_WIDTH = 256;
  constexpr int GRID_HEIGHT = 192;
  constexpr int CELL_SCALE = 4;

  constexpr int WINDOW_WIDTH = GRID_WIDTH * CELL_SCALE;
  constexpr int WINDOW_HEIGHT = GRID_HEIGHT * CELL_SCALE;

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Falling Sand");

  if (!IsWindowReady()) {
    TraceLog(LOG_ERROR, "Failed to create the window");
    return 1;
  }

  SetTargetFPS(60);

  int exitCode = 0;

  try {
    sand::Simulation simulation(GRID_WIDTH, GRID_HEIGHT);
    sand::RaylibView view(simulation, CELL_SCALE);

    while (!WindowShouldClose()) {
      BeginDrawing();
      ClearBackground(BLACK);
      view.draw(simulation);
      EndDrawing();
    }
  } catch (const std::exception &error) {
    TraceLog(LOG_ERROR, "%s", error.what());
    exitCode = 1;
  }

  CloseWindow();
  return exitCode;
}
