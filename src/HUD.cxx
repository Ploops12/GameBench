#include "HUD.h"

void HUD::draw(const Player &player, bool coatOpen, int enemiesRemaining) const {
	const int w = GetScreenWidth();
	const int h = GetScreenHeight();
	DrawRectangle(18, h - 92, 248, 70, Color{10, 14, 20, 220});
	DrawRectangleLines(18, h - 92, 248, 70, Color{220, 190, 110, 255});
	DrawText("VITAL", 30, h - 82, 12, LIGHTGRAY);
	DrawRectangle(30, h - 62, 180, 12, Color{50, 30, 35, 255});
	DrawRectangle(30, h - 62, 180 * player.getHealth() / player.getMaxHealth(), 12, Color{220, 70, 80, 255});
	DrawText(TextFormat("%d / %d", player.getHealth(), player.getMaxHealth()), 218, h - 64, 12, RAYWHITE);
	DrawText("WARD", 30, h - 43, 12, LIGHTGRAY);
	DrawRectangle(30, h - 27, 180, 8, Color{30, 45, 55, 255});
	DrawRectangle(30, h - 27, static_cast<int>(180.0f * player.getWard() / player.getMaxWard()), 8, Color{100, 210, 240, 255});
	DrawText(player.isWardActive() ? "ACTIVE" : "R: WARD", 218, h - 30, 12, player.isWardActive() ? SKYBLUE : RAYWHITE);
	DrawText(TextFormat("HUNTERS %d", enemiesRemaining), w - 150, 24, 16, Color{255, 210, 130, 255});
	DrawLine(w / 2 - 10, h / 2, w / 2 + 10, h / 2, RAYWHITE);
	DrawLine(w / 2, h / 2 - 10, w / 2, h / 2 + 10, RAYWHITE);
	if (!coatOpen) {
		DrawText("LMB CAST", w - 150, h - 58, 14, RAYWHITE);
		DrawText("TAB COAT", w - 150, h - 36, 14, Color{255, 220, 140, 255});
	}
}
