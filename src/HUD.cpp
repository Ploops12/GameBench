#include <cstdio>
#include <string>
#include <vector>
#include <raylib.h>
#include "HUD.h"
#include "Player.hxx"

namespace {
	void drawMeter(int x, int y, int width, int height, float value, float maxValue, Color fill, const char* label) {
		DrawRectangleLines(x, y, width, height, Color {160, 190, 220, 180});
		const float ratio = maxValue > 0.0f ? value / maxValue : 0.0f;
		DrawRectangle(x + 2, y + 2, static_cast<int>((width - 4) * ratio), height - 4, fill);
		DrawText(label, x, y - 22, 18, Color {210, 220, 235, 255});
	}
}

void HUD::draw(const Player& player, int screenWidth, int screenHeight, int wave, int enemiesAlive, bool cleared) const {
	drawMeter(24, 26, 260, 24, player.getHealth(), player.getMaxHealth(), Color {195, 84, 96, 255}, "Health");
	drawMeter(24, 76, 260, 24, player.getWard(), player.getMaxWard(), Color {80, 170, 255, 255}, "Ward");
	DrawText(player.isWardActive() ? "WARD UP" : "WARD DOWN", 24, 110, 18, player.isWardActive() ? Color {120, 210, 255, 255} : Color {170, 182, 198, 255});

	char text[128];
	std::snprintf(text, sizeof(text), "Wave %d  Enemies %d", wave, enemiesAlive);
	DrawText(text, screenWidth / 2 - 90, 24, 24, Color {215, 225, 240, 255});
	if (cleared) {
		DrawText("Lane cleared", screenWidth / 2 - 70, 56, 20, Color {255, 194, 104, 255});
	}

	const SpellPreview preview = player.previewSpell();
	std::vector<std::string> lines = DescribeSpell(preview);
	DrawRectangleLines(20, screenHeight - 180, 330, 150, Color {140, 170, 205, 180});
	DrawText("Current Hex", 32, screenHeight - 172, 20, Color {230, 235, 245, 255});
	for (std::size_t i = 0; i < lines.size() && i < 6; ++i) {
		DrawText(lines[i].c_str(), 32, screenHeight - 145 + static_cast<int>(i) * 22, 18, Color {205, 215, 232, 255});
	}

	DrawText("R hold: coat rummage", screenWidth - 250, 28, 20, Color {205, 215, 232, 255});
	DrawText("LMB cast / RMB ward", screenWidth - 250, 52, 20, Color {205, 215, 232, 255});
	if (player.isRummaging()) {
		DrawText("Rummaging: slowed and exposed", screenWidth - 290, 78, 20, Color {255, 132, 132, 255});
	}

	const int centerX = screenWidth / 2;
	const int centerY = screenHeight / 2;
	DrawCircleLines(centerX, centerY, 14.0f, Color {220, 230, 245, 170});
	DrawLine(centerX - 18, centerY, centerX + 18, centerY, Color {220, 230, 245, 150});
	DrawLine(centerX, centerY - 18, centerX, centerY + 18, Color {220, 230, 245, 150});
}
