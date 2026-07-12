#include "MainMenu.h"

#include <algorithm>
#include <cmath>

#include <raylib.h>

namespace {

void drawCentredText(const char* text, int y, int fontSize, Color color) {
	const int x = (GetScreenWidth() - MeasureText(text, fontSize)) / 2;
	DrawText(text, x, y, fontSize, color);
}

} // namespace

MainMenu::MainMenu() {
	setActive(true);
}

bool MainMenu::updateAndCheckStart() {
	if (!active) return false;

	const bool startPressed = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
		IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT);
	if (startPressed) setActive(false);
	return startPressed;
}

void MainMenu::draw() const {
	if (!active) return;

	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	const int panelWidth = std::min(width - 48, 720);
	const int panelHeight = std::min(height - 48, 600);
	const Rectangle panel {
		static_cast<float>((width - panelWidth) / 2),
		static_cast<float>((height - panelHeight) / 2),
		static_cast<float>(panelWidth),
		static_cast<float>(panelHeight)
	};
	const int centreX = width / 2;

	DrawRectangle(0, 0, width, height, Color {8, 10, 13, 255});
	DrawRectangleRec(panel, Color {13, 17, 20, 255});
	DrawRectangleLinesEx(panel, 2.0f, Color {198, 225, 211, 255});
	DrawLine(centreX - panelWidth / 2 + 24, static_cast<int>(panel.y) + 112,
		centreX + panelWidth / 2 - 24, static_cast<int>(panel.y) + 112, Color {104, 154, 137, 255});

	drawCentredText("THE SALEM HEX", static_cast<int>(panel.y) + 38, 42,
		Color {224, 238, 225, 255});
	drawCentredText("A FIRST-PERSON WITCH HUNT", static_cast<int>(panel.y) + 84, 16,
		Color {122, 186, 161, 255});

	const int controlsY = static_cast<int>(panel.y) + 150;
	drawCentredText("WASD - MOVE  |  MOUSE - AIM", controlsY, 18, RAYWHITE);
	drawCentredText("SPACE - JUMP  |  SHIFT - SPRINT", controlsY + 34, 18, RAYWHITE);
	drawCentredText("LMB - CAST  |  RMB - WARD/BLOCK", controlsY + 68, 18, RAYWHITE);
	drawCentredText("R (HOLD) / TAB (TOGGLE) - SPELL SELECT", controlsY + 102, 18,
		Color {232, 188, 96, 255});
	drawCentredText("MAIN: 1-4 - SELECT  |  Q/E - CYCLE", controlsY + 144, 14,
		Color {222, 127, 105, 255});

	const float pulse = 0.68f + 0.32f * std::sin(static_cast<float>(GetTime()) * 4.0f);
	drawCentredText("PRESS ENTER OR CLICK TO BEGIN", static_cast<int>(panel.y + panel.height) - 70,
		20, Fade(Color {224, 238, 225, 255}, pulse));
}
