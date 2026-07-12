#include "HUD.h"
#include <algorithm>

namespace {
void DrawMeter(int x, int y, int w, int h, float value, float maxValue, Color color, const char* label) {
	float t = maxValue > 0.0f ? std::clamp(value / maxValue, 0.0f, 1.0f) : 0.0f;
	DrawRectangleLines(x, y, w, h, Color{220, 235, 210, 255});
	DrawRectangle(x + 3, y + 3, static_cast<int>((w - 6) * t), h - 6, color);
	DrawText(TextFormat("%s %.0f/%.0f", label, value, maxValue), x + 8, y + 6, 14, Color{245, 250, 220, 255});
}
}

void HUD::draw(const Player& player, int enemyCount, int screenWidth, int screenHeight) const {
	DrawMeter(20, 20, 188, 28, player.health, player.maxHealth, Color{172, 38, 34, 255}, "BLOOD");
	DrawMeter(20, 54, 188, 28, player.ward, player.maxWard, player.wardActive ? Color{95, 210, 255, 255} : Color{64, 115, 155, 255}, "WARD");

	Color wardColor = player.wardActive ? Color{125, 235, 255, 220} : Color{85, 120, 130, 160};
	DrawCircleLines(screenWidth - 76, 58, 34.0f, wardColor);
	DrawCircleLines(screenWidth - 76, 58, 24.0f, wardColor);
	DrawText("RMB", screenWidth - 92, 49, 16, wardColor);

	DrawLine(screenWidth / 2 - 11, screenHeight / 2, screenWidth / 2 - 3, screenHeight / 2, Color{245, 235, 205, 255});
	DrawLine(screenWidth / 2 + 3, screenHeight / 2, screenWidth / 2 + 11, screenHeight / 2, Color{245, 235, 205, 255});
	DrawLine(screenWidth / 2, screenHeight / 2 - 11, screenWidth / 2, screenHeight / 2 - 3, Color{245, 235, 205, 255});
	DrawLine(screenWidth / 2, screenHeight / 2 + 3, screenWidth / 2, screenHeight / 2 + 11, Color{245, 235, 205, 255});

	std::string preview = BuildSpellPreview(player.draft);
	DrawRectangle(20, screenHeight - 86, 420, 60, Color{6, 11, 9, 190});
	DrawRectangleLines(20, screenHeight - 86, 420, 60, Color{220, 235, 210, 255});
	DrawText(TextFormat("CAST LMB: %s", preview.c_str()), 32, screenHeight - 74, 15, Color{245, 250, 220, 255});
	DrawText("E coat rummage  |  Q base while rummaging  |  Poppet works empty", 32, screenHeight - 48, 13, Color{150, 180, 150, 255});

	DrawText(TextFormat("HUNTERS %d", enemyCount), screenWidth - 152, 20, 18, Color{255, 190, 120, 255});

	if (player.wardFlash > 0.0f) {
		DrawCircleLines(screenWidth / 2, screenHeight / 2, 96.0f, Color{110, 230, 255, 180});
	}
	if (player.hurtFlash > 0.0f) {
		DrawRectangle(0, 0, screenWidth, screenHeight, Color{160, 20, 20, 70});
	}
}
