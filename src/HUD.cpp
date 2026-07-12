#include "HUD.h"
#include "Player.hxx"
#include <raylib.h>
#include <string>

namespace {
void meter(int x, int y, int width, const char* label, float value, float maximum, Color color) {
	DrawText(label, x, y, 16, color);
	DrawRectangleLines(x, y + 21, width, 12, color);
	DrawRectangle(x + 2, y + 23, static_cast<int>((width - 4) * value / maximum), 8, Fade(color, 0.75f));
	DrawText(TextFormat("%d / %d", static_cast<int>(value), static_cast<int>(maximum)), x + width + 9, y + 18, 14, color);
}

void drawHands(const Player& player) {
	const int w = GetScreenWidth();
	const int h = GetScreenHeight();
	const float castOffset = player.getCastFlash() > 0.0f ? -28.0f : 0.0f;
	const Color hand = player.isWardActive() ? Color{105, 225, 235, 255} : Color{205, 210, 190, 255};
	Vector2 left[5] = {
		{w * 0.26f, h * 0.98f}, {w * 0.31f, h * 0.82f + castOffset}, {w * 0.37f, h * 0.77f + castOffset},
		{w * 0.40f, h * 0.84f + castOffset}, {w * 0.36f, h * 0.98f}
	};
	Vector2 right[5] = {
		{w * 0.74f, h * 0.98f}, {w * 0.69f, h * 0.82f + castOffset}, {w * 0.63f, h * 0.77f + castOffset},
		{w * 0.60f, h * 0.84f + castOffset}, {w * 0.64f, h * 0.98f}
	};
	for (int i = 0; i < 5; ++i) {
		DrawLineEx(left[i], left[(i + 1) % 5], 3.0f, hand);
		DrawLineEx(right[i], right[(i + 1) % 5], 3.0f, hand);
	}
	for (int i = 0; i < 3; ++i) {
		DrawLineEx(left[2], Vector2{left[2].x - 20.0f + i * 11.0f, left[2].y - 35.0f - i * 4.0f}, 2.0f, hand);
		DrawLineEx(right[2], Vector2{right[2].x + 20.0f - i * 11.0f, right[2].y - 35.0f - i * 4.0f}, 2.0f, hand);
	}
}
}

void HUD::draw(const Player& player, int enemiesRemaining, int collected,
	const char* message, float messageTimer, bool rummaging) const {
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	meter(22, 20, 150, "LIFE", player.getHealth(), player.getMaxHealth(), Color{235, 90, 75, 255});
	meter(22, 66, 150, player.isWardActive() ? "WARD // HELD" : "WARD [F]", player.getWard(), player.getMaxWard(), Color{95, 215, 225, 255});
	DrawText(TextFormat("HUNTERS %d", enemiesRemaining), width - 145, 22, 18, Color{235, 175, 75, 255});
	DrawText(TextFormat("REAGENTS %d", collected), width - 145, 47, 16, Color{180, 205, 180, 255});
	DrawText(player.getSpell().preview().c_str(), 22, height - 44, 16, Color{235, 205, 100, 255});
	if (!rummaging) DrawText("HOLD Q: COAT  |  LMB: CAST  |  F: WARD", width - 390, height - 42, 15, Color{180, 205, 180, 255});

	const int cx = width / 2;
	const int cy = height / 2;
	const Color cross = player.isWardActive() ? Color{100, 225, 235, 255} : Color{235, 205, 100, 255};
	DrawLine(cx - 9, cy, cx - 3, cy, cross);
	DrawLine(cx + 3, cy, cx + 9, cy, cross);
	DrawLine(cx, cy - 9, cx, cy - 3, cross);
	DrawLine(cx, cy + 3, cx, cy + 9, cross);

	drawHands(player);
	if (player.isWardActive()) {
		DrawRing(Vector2{static_cast<float>(cx), static_cast<float>(cy)}, 125.0f, 129.0f, 0.0f, 360.0f, 48, Fade(Color{95, 220, 235, 255}, 0.8f));
		DrawRectangleLinesEx(Rectangle{5, 5, static_cast<float>(width - 10), static_cast<float>(height - 10)}, 3.0f, Fade(Color{95, 220, 235, 255}, 0.55f));
	}
	if (player.getDamageFlash() > 0.0f) DrawRectangle(0, 0, width, height, Fade(Color{210, 35, 35, 255}, player.getDamageFlash() * 0.5f));
	if (messageTimer > 0.0f && message && message[0]) {
		const int tw = MeasureText(message, 22);
		DrawRectangle(cx - tw / 2 - 12, 118, tw + 24, 38, Color{8, 10, 12, 210});
		DrawRectangleLines(cx - tw / 2 - 12, 118, tw + 24, 38, Color{215, 195, 120, 255});
		DrawText(message, cx - tw / 2, 126, 22, Color{235, 215, 150, 255});
	}
}
