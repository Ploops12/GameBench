#include "MainMenu.h"

#include <cmath>

bool Menu::isActive() const {
	return active;
}

void Menu::setActive(bool value) {
	active = value;
}

void MainMenu::update(float dt) {
	if (!active) return;
	pulse += dt;
	const Rectangle button = playButton();
	if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
		(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), button))) {
		playRequested = true;
	}
}

void MainMenu::draw() const {
	BeginDrawing();
	ClearBackground(Color{5, 7, 9, 255});

	const int sw = GetScreenWidth();
	const int sh = GetScreenHeight();
	const Color line{222, 239, 222, 255};
	const Color green{108, 247, 151, 255};
	const Color red{238, 68, 83, 255};
	const float wobble = std::sin(pulse * 3.0f) * 4.0f;

	DrawRectangleLines(38, 38, sw - 76, sh - 76, line);
	DrawRectangleLines(48, 48, sw - 96, sh - 96, Color{80, 120, 96, 255});
	DrawText("SALEM HEXSHOOTER", sw / 2 - MeasureText("SALEM HEXSHOOTER", 42) / 2, 116, 42, green);
	DrawText("first-person witch combat slice", sw / 2 - MeasureText("first-person witch combat slice", 18) / 2, 166, 18, line);

	Vector2 leftHand{sw * 0.33f, sh * 0.47f + wobble};
	Vector2 rightHand{sw * 0.67f, sh * 0.47f - wobble};
	DrawCircleLines(static_cast<int>(leftHand.x), static_cast<int>(leftHand.y), 36.0f, line);
	DrawCircleLines(static_cast<int>(rightHand.x), static_cast<int>(rightHand.y), 36.0f, line);
	for (int i = 0; i < 5; ++i) {
		const float offset = (i - 2) * 12.0f;
		DrawLineEx({leftHand.x + offset, leftHand.y - 24.0f}, {leftHand.x + offset - 22.0f, leftHand.y - 80.0f - i * 2.0f}, 3.0f, line);
		DrawLineEx({rightHand.x + offset, rightHand.y - 24.0f}, {rightHand.x + offset + 22.0f, rightHand.y - 80.0f - i * 2.0f}, 3.0f, line);
	}

	DrawCircleLines(sw / 2, static_cast<int>(sh * 0.48f), 78.0f, red);
	DrawLine(sw / 2 - 55, static_cast<int>(sh * 0.48f), sw / 2 + 55, static_cast<int>(sh * 0.48f), red);
	DrawLine(sw / 2, static_cast<int>(sh * 0.48f) - 55, sw / 2, static_cast<int>(sh * 0.48f) + 55, red);

	const Rectangle button = playButton();
	DrawRectangleRec(button, Fade(Color{0, 0, 0, 255}, 0.7f));
	DrawRectangleLinesEx(button, 2.0f, green);
	DrawText("PLAY", static_cast<int>(button.x + button.width * 0.5f - MeasureText("PLAY", 28) * 0.5f), static_cast<int>(button.y + 17), 28, green);
	DrawText("Enter / Space / Click", sw / 2 - MeasureText("Enter / Space / Click", 16) / 2, static_cast<int>(button.y + button.height + 18), 16, line);
	DrawText("WASD move  TAB rummage  LMB cast  RMB/F ward", sw / 2 - MeasureText("WASD move  TAB rummage  LMB cast  RMB/F ward", 16) / 2, sh - 104, 16, Color{180, 210, 184, 255});

	EndDrawing();
}

bool MainMenu::consumePlayRequested() {
	if (!playRequested) return false;
	playRequested = false;
	active = false;
	return true;
}

void MainMenu::requestPlay() {
	playRequested = true;
}

Rectangle MainMenu::playButton() const {
	const float width = 220.0f;
	const float height = 68.0f;
	return {GetScreenWidth() * 0.5f - width * 0.5f, GetScreenHeight() * 0.68f, width, height};
}
