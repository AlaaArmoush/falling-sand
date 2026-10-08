#include "platform/RaylibView.hpp"
#include "raylib.h"
#include "simulation/Simulation.hpp"

#include <exception>

int main() {
  constexpr int GRID_WIDTH = 256;
  constexpr int GRID_HEIGHT = 192;
  constexpr int CELL_SCALE = 4;
  constexpr int BRUSH_RADIUS = 3;
  constexpr double BRUSH_PLACEMENT_PROBABILITY = 0.5;

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

    bool paused = false;
    SetWindowTitle("Falling Sand - Running");

    while (!WindowShouldClose()) {
      const sand::InputCommands commands = view.pollInput();

      if (commands.pauseToggleRequested) {
        paused = !paused;
        SetWindowTitle(paused ? "Falling Sand - Paused (N: single step)"
                              : "Falling Sand - Running");
      }

      if (commands.clearRequested) {
        simulation.clear();
      } else if (commands.paintPosition.has_value()) {
        const sand::GridPosition position = commands.paintPosition.value();
        simulation.placeSandBrush(position.x, position.y, BRUSH_RADIUS,
                                  BRUSH_PLACEMENT_PROBABILITY);
      }

      if (!paused) {
        simulation.step();
      } else if (commands.singleStepRequested) {
        simulation.step();
      }

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
