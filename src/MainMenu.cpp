#include "MainMenu.h"

#include <raylib.h>

bool MainMenu::update(const InputHandler::InputState& input) {
	if (open && input.startPressed) {
		open = false;
	}

	return !open;
}

void MainMenu::draw(int screenWidth, int screenHeight) const {
	if (!open) {
		return;
	}

	const Color background {5, 9, 16, 238};
	const Color ink {218, 241, 232, 255};
	const Color accent {129, 255, 196, 255};
	const Color warning {255, 132, 116, 255};

	DrawRectangle(0, 0, screenWidth, screenHeight, background);
	DrawRectangleLinesEx(Rectangle {32.0f, 32.0f, static_cast<float>(screenWidth - 64), static_cast<float>(screenHeight - 64)}, 2.0f, accent);
	DrawLine(screenWidth / 2 - 210, 150, screenWidth / 2 + 210, 150, accent);
	DrawCircleLines(screenWidth / 2, 150, 32.0f, warning);
	DrawCircleLines(screenWidth / 2, 150, 24.0f, accent);

	const char* title = "SALEM: SATCHEL OF THORNS";
	DrawText(title, screenWidth / 2 - MeasureText(title, 34) / 2, 205, 34, ink);
	const char* subtitle = "A first-person hex-shooter vertical slice";
	DrawText(subtitle, screenWidth / 2 - MeasureText(subtitle, 18) / 2, 250, 18, accent);

	const int left = screenWidth / 2 - 255;
	DrawText("WASD  move       SPACE  jump       SHIFT  sprint", left, 330, 19, ink);
	DrawText("LMB  cast        F / RMB  ward", left, 365, 19, ink);
	DrawText("HOLD R  coat rummage     TAB  toggle coat", left, 400, 19, ink);
	DrawText("1-4  base vector     Z X C V B N  reagent", left, 435, 19, ink);
	DrawText("ENTER / click to start", screenWidth / 2 - 126, screenHeight - 142, 22, warning);
	DrawText("Enemies, drops, and time keep moving while the coat is open.",
		screenWidth / 2 - 266, screenHeight - 100, 16, accent);
}

bool MainMenu::isOpen() const {
	return open;
}
