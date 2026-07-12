#pragma once

#include <raylib.h>

class MainMenu {
public:
	bool update() const { return IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT); }
	void draw() const {
		const int w = GetScreenWidth();
		const int h = GetScreenHeight();
		DrawRectangle(0, 0, w, h, Color{13, 19, 27, 255});
		DrawRectangleLinesEx(Rectangle{42, 42, static_cast<float>(w - 84), static_cast<float>(h - 84)}, 3.0f, Color{220, 190, 110, 255});
		DrawText("SALEM HEXSHOOTER", w / 2 - 190, h / 2 - 90, 36, Color{255, 230, 160, 255});
		DrawText("A WITCH'S LAST WALK", w / 2 - 104, h / 2 - 42, 16, Color{160, 220, 190, 255});
		DrawText("PRESS ENTER OR CLICK TO ENTER SALEM", w / 2 - 166, h / 2 + 65, 16, RAYWHITE);
		DrawText("WASD  MOVE     MOUSE  LOOK     TAB  RUMMAGE     R  WARD", w / 2 - 222, h - 82, 12, LIGHTGRAY);
	}
};
