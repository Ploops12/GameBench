#include "MainMenu.h"

#include <raylib.h>

MainMenu::MainMenu() {
	open = true;
}

bool MainMenu::update(const InputHandler::InputState& input) {
	if (input.confirmPressed || input.castPressed) {
		open = false;
		return true;
	}
	return false;
}

void MainMenu::draw(int screenWidth, int screenHeight) const {
	const Color ink {8, 12, 20, 255};
	const Color cyan {134, 232, 255, 255};
	const Color rose {255, 106, 106, 255};

	DrawRectangle(0, 0, screenWidth, screenHeight, ink);
	DrawRectangleLinesEx({24.0f, 24.0f, static_cast<float>(screenWidth - 48), static_cast<float>(screenHeight - 48)}, 2.0f, cyan);

	for (int index = 0; index < 9; ++index) {
		const float x = 90.0f + static_cast<float>(index) * 132.0f;
		DrawLineEx({x, 88.0f}, {x + 38.0f, 125.0f}, 2.0f, Fade(cyan, 0.45f));
		DrawCircleLines(static_cast<int>(x + 19.0f), 105, 13.0f, Fade(rose, 0.65f));
	}

	const char* title = "SALEM: SATCHEL OF THORNS";
	const int titleSize = 42;
	DrawText(title, (screenWidth - MeasureText(title, titleSize)) / 2, screenHeight / 2 - 150, titleSize, cyan);
	DrawText("A FIRST-PERSON HEX SHOOTER", (screenWidth - MeasureText("A FIRST-PERSON HEX SHOOTER", 20)) / 2,
		screenHeight / 2 - 95, 20, Fade(rose, 0.9f));

	DrawRectangleLinesEx({static_cast<float>(screenWidth / 2 - 238), static_cast<float>(screenHeight / 2 - 38), 476.0f, 236.0f}, 2.0f, Fade(cyan, 0.75f));
	DrawText("HUNT THROUGH A HOSTILE SALEM LANE.", screenWidth / 2 - 200, screenHeight / 2 - 13, 20, RAYWHITE);
	DrawText("Collect reagents from fallen hunters.", screenWidth / 2 - 200, screenHeight / 2 + 28, 18, LIGHTGRAY);
	DrawText("Hold R to rummage without pausing the hunt.", screenWidth / 2 - 200, screenHeight / 2 + 58, 18, LIGHTGRAY);
	DrawText("LMB cast  /  RMB ward  /  WASD move  /  SPACE jump", screenWidth / 2 - 200, screenHeight / 2 + 108, 16, cyan);

	const char* begin = "[ ENTER OR CLICK ]  BEGIN HUNT";
	DrawText(begin, (screenWidth - MeasureText(begin, 24)) / 2, screenHeight / 2 + 250, 24, rose);
}
