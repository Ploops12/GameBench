#include "MainMenu.h"
#include <raylib.h>

bool MainMenu::update(const InputHandler::InputState& input) {
	visible = true;
	return input.startPressed;
}

void MainMenu::draw(int screenWidth, int screenHeight) const {
	ClearBackground(Color {9, 11, 16, 255});

	const int cx = screenWidth / 2;
	const int cy = screenHeight / 2;

	DrawRing(Vector2 {static_cast<float>(cx), static_cast<float>(cy - 80)}, 90.0f, 110.0f, 0.0f, 360.0f, 64, Color {110, 180, 255, 70});
	DrawLine(cx - 110, cy - 80, cx + 110, cy - 80, Color {190, 220, 255, 180});
	DrawLine(cx, cy - 190, cx, cy + 30, Color {190, 220, 255, 180});

	DrawText("SALEM HEXSHOOTER", cx - 210, cy - 190, 40, Color {230, 240, 255, 255});
	DrawText("first-person witch combat slice", cx - 195, cy - 138, 24, Color {180, 205, 230, 255});

	DrawRectangleLines(cx - 250, cy - 30, 500, 180, Color {130, 170, 210, 180});
	DrawText("WASD move   Mouse look   Shift sprint   Space jump", cx - 220, cy - 5, 22, Color {210, 220, 235, 255});
	DrawText("LMB cast   RMB ward   Hold R rummage", cx - 220, cy + 35, 22, Color {210, 220, 235, 255});
	DrawText("1-4 base vectors   Q/W/E/A/S/D reagents", cx - 220, cy + 75, 22, Color {210, 220, 235, 255});
	DrawText("Press Enter to step into Salem", cx - 180, cy + 140, 28, Color {255, 184, 110, 255});
}
