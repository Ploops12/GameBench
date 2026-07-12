#include "MainMenu.h"
#include <cmath>
#include <raylib.h>

void MainMenu::update() {
	if (!active) return;
	pulse += GetFrameTime();
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	const Rectangle playButton{width * 0.5f - 135.0f, height * 0.63f, 270.0f, 54.0f};
	if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
		(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), playButton))) {
		playRequested = true;
	}
}

bool MainMenu::takePlayRequest() {
	const bool result = playRequested;
	playRequested = false;
	if (result) active = false;
	return result;
}

void MainMenu::draw() const {
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	ClearBackground(Color{5, 8, 10, 255});
	const Color pale{185, 215, 190, 255};
	const Color ember{235, 90, 75, 255};
	for (int i = 0; i < 9; ++i) {
		const int x = width / 2 - 320 + i * 80;
		DrawLine(x, height / 2 + 105, width / 2, height / 2 - 150, Fade(pale, 0.18f));
	}
	DrawRing(Vector2{width * 0.5f, height * 0.39f}, 86.0f, 89.0f, 0.0f, 360.0f, 48, pale);
	DrawLine(width / 2, static_cast<int>(height * 0.39f - 62), width / 2, static_cast<int>(height * 0.39f + 62), ember);
	DrawLine(width / 2 - 50, static_cast<int>(height * 0.39f + 32), width / 2 + 50, static_cast<int>(height * 0.39f - 32), ember);
	DrawLine(width / 2 - 50, static_cast<int>(height * 0.39f - 32), width / 2 + 50, static_cast<int>(height * 0.39f + 32), ember);

	const char* title = "SATCHEL OF THORNS";
	const char* subtitle = "A SALEM HEXSHOOTER";
	DrawText(title, width / 2 - MeasureText(title, 42) / 2, 72, 42, ember);
	DrawText(subtitle, width / 2 - MeasureText(subtitle, 20) / 2, 126, 20, pale);
	const Rectangle playButton{width * 0.5f - 135.0f, height * 0.63f, 270.0f, 54.0f};
	DrawRectangleRec(playButton, Color{8, 13, 14, 255});
	DrawRectangleLinesEx(playButton, 2.0f + std::sin(pulse * 3.0f) * 0.5f, Color{235, 205, 95, 255});
	const char* play = "ENTER THE ROAD";
	DrawText(play, width / 2 - MeasureText(play, 22) / 2, static_cast<int>(playButton.y + 16), 22, Color{235, 205, 95, 255});
	const char* prompt = "ENTER / SPACE / CLICK";
	DrawText(prompt, width / 2 - MeasureText(prompt, 16) / 2, static_cast<int>(playButton.y + 70), 16, Fade(pale, 0.75f));
	DrawText("WASD MOVE  //  MOUSE LOOK  //  Q COAT  //  F WARD  //  LMB CAST",
		width / 2 - MeasureText("WASD MOVE  //  MOUSE LOOK  //  Q COAT  //  F WARD  //  LMB CAST", 16) / 2,
		height - 54, 16, pale);
}
