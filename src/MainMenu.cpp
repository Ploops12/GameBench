#include "MainMenu.h"

#include <raylib.h>

#include <cmath>

MainMenu::MainMenu() {
	active_ = true;
}

bool MainMenu::update(const InputHandler::InputState& input) {
	age_ += GetFrameTime();
	return input.confirmPressed || input.castPressed;
}

void MainMenu::draw() const {
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	ClearBackground({5, 8, 13, 255});

	const Color cyan {95, 224, 226, 255};
	const Color ember {255, 93, 82, 255};
	const Color dim {42, 99, 112, 255};
	const int horizon = static_cast<int>(height * 0.72f);
	DrawCircleLines(static_cast<int>(width * 0.78f), static_cast<int>(height * 0.22f), height * 0.075f, {188, 226, 214, 200});
	for (int x = 0; x < width; x += 95) {
		const int buildingHeight = 70 + (x / 95 % 4) * 24;
		DrawRectangleLines(x, horizon - buildingHeight, 82, buildingHeight, dim);
		DrawTriangleLines({static_cast<float>(x - 5), static_cast<float>(horizon - buildingHeight)},
			{static_cast<float>(x + 41), static_cast<float>(horizon - buildingHeight - 35)},
			{static_cast<float>(x + 87), static_cast<float>(horizon - buildingHeight)}, dim);
	}
	DrawLine(0, horizon, width, horizon, cyan);
	for (int x = 0; x < width; x += 40) {
		DrawLine(x, horizon, width / 2, height, ColorAlpha(dim, 0.45f));
	}

	const char* title = "THORNWAKE";
	const char* subtitle = "A SALEM HEXSHOOTER";
	const int titleSize = width > 900 ? 72 : 48;
	DrawText(title, width / 2 - MeasureText(title, titleSize) / 2 + 3, static_cast<int>(height * 0.18f) + 3, titleSize, ColorAlpha(ember, 0.28f));
	DrawText(title, width / 2 - MeasureText(title, titleSize) / 2, static_cast<int>(height * 0.18f), titleSize, cyan);
	DrawText(subtitle, width / 2 - MeasureText(subtitle, 22) / 2, static_cast<int>(height * 0.18f) + titleSize + 10, 22, ember);

	const int panelWidth = std::min(640, width - 60);
	const int panelX = width / 2 - panelWidth / 2;
	const int panelY = static_cast<int>(height * 0.42f);
	DrawRectangle(panelX, panelY, panelWidth, 142, {4, 8, 12, 225});
	DrawRectangleLinesEx({static_cast<float>(panelX), static_cast<float>(panelY), static_cast<float>(panelWidth), 142.0f}, 2.0f, dim);
	DrawText("WASD MOVE   SHIFT SPRINT   SPACE JUMP   MOUSE LOOK", panelX + 24, panelY + 22, 18, {214, 229, 222, 255});
	DrawText("LMB CAST    RMB / Q WARD    HOLD R RUMMAGE", panelX + 24, panelY + 54, 18, {214, 229, 222, 255});
	DrawText("IN COAT: 1-4 VECTOR   Z X C V B N REAGENTS", panelX + 24, panelY + 86, 18, {214, 229, 222, 255});

	const float pulse = 0.55f + std::sin(age_ * 3.0f) * 0.25f;
	const char* prompt = "PRESS ENTER OR LEFT CLICK TO ENTER SALEM";
	DrawText(prompt, width / 2 - MeasureText(prompt, 21) / 2, panelY + 174, 21, ColorAlpha(ember, pulse));
	DrawText("ESC releases / recaptures the cursor", width / 2 - MeasureText("ESC releases / recaptures the cursor", 15) / 2,
		height - 35, 15, {120, 154, 158, 255});
}
