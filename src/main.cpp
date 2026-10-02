#include "raylib.h"

int main() {
  constexpr int WIDTH = 800;
  constexpr int HEIGHT = 600;

  InitWindow(WIDTH, HEIGHT, "Falling Sand");
  SetTargetFPS(60);

  bool greenSelected = false;
  while (!WindowShouldClose()) {
    const Vector2 mousePosition = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      greenSelected = !greenSelected;
    }

    const bool leftButtonHeld = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    const Color panelColor = greenSelected ? GREEN : SKYBLUE;

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(240, 200, 520, 200, panelColor);
    DrawText("Drawing Happens in order", 300, 280, 30, DARKGRAY);
    DrawText(leftButtonHeld ? "Left button: HELD" : "Left button: released",
             300, 330, 20, leftButtonHeld ? MAROON : DARKBLUE);
    DrawText(TextFormat("Mouse: %0.f, %0.f", mousePosition.x, mousePosition.y),
             10, 10, 20, BLACK);
    DrawCircleV(mousePosition, 8.0F, RED);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
