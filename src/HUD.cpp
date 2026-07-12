#include "HUD.h"

#include <algorithm>
#include <cstdio>

#include <raylib.h>

#include "CoatMenu.h"
#include "Player.hxx"

namespace {
void meter(Rectangle bounds, float value, float maximum, Color color, const char* label) {
	float ratio = maximum > 0.0f ? std::clamp(value / maximum, 0.0f, 1.0f) : 0.0f;
	DrawText(label, static_cast<int>(bounds.x), static_cast<int>(bounds.y - 19), 15, RAYWHITE);
	DrawRectangleRec(bounds, Fade(BLACK, 0.75f));
	DrawRectangleLinesEx(bounds, 2.0f, color);
	DrawRectangle(static_cast<int>(bounds.x + 3), static_cast<int>(bounds.y + 3), static_cast<int>((bounds.width - 6) * ratio), static_cast<int>(bounds.height - 6), Fade(color, 0.9f));
	char number[32];
	std::snprintf(number, sizeof(number), "%.0f / %.0f", value, maximum);
	DrawText(number, static_cast<int>(bounds.x + bounds.width + 9), static_cast<int>(bounds.y - 1), 14, color);
}
}

void HUD::draw(const Player& player, const CoatMenu& coat, int enemiesRemaining) const {
	meter({22.0f, 39.0f, 184.0f, 17.0f}, player.getHealth(), player.getMaxHealth(), {229, 91, 108, 255}, "VITALITY");
	Color wardColor = player.isWardActive() ? Color {111, 216, 237, 255} : Color {82, 155, 169, 255};
	meter({22.0f, 87.0f, 184.0f, 17.0f}, player.getWard(), player.getMaxWard(), wardColor, player.isWardActive() ? "WARD // HELD" : "WARD // F TO RAISE");

	char count[64];
	std::snprintf(count, sizeof(count), "HOSTILES %d", enemiesRemaining);
	DrawText(count, GetScreenWidth() - 154, 24, 16, {233, 222, 189, 255});
	DrawText("LMB CAST  R COAT  F WARD", GetScreenWidth() - 242, 47, 13, LIGHTGRAY);
	if (!coat.isOpen()) DrawText("HOLD R: RUMMAGE IN REAL TIME", GetScreenWidth() - 277, GetScreenHeight() - 32, 14, {174, 132, 239, 255});

	int centerX = GetScreenWidth() / 2;
	int centerY = GetScreenHeight() / 2;
	DrawCircleLines(centerX, centerY, 7.0f, player.isWardActive() ? wardColor : RAYWHITE);
	DrawLine(centerX - 13, centerY, centerX - 4, centerY, RAYWHITE);
	DrawLine(centerX + 4, centerY, centerX + 13, centerY, RAYWHITE);
	DrawLine(centerX, centerY - 13, centerX, centerY - 4, RAYWHITE);
	DrawLine(centerX, centerY + 4, centerX, centerY + 13, RAYWHITE);

	if (player.isWardActive() || player.getWardFlash() > 0.0f) {
		float alpha = player.isWardActive() ? 0.32f : player.getWardFlash() * 0.28f;
		DrawCircleLines(centerX, centerY, 30.0f, Fade(wardColor, alpha));
		DrawCircleLines(centerX, centerY, 46.0f, Fade(wardColor, alpha));
	}
}

void HUD::drawHands(const Player& player, bool casting) const {
	int width = GetScreenWidth();
	int height = GetScreenHeight();
	Color hand = player.isWardActive() ? Color {111, 216, 237, 255} : Color {233, 222, 189, 255};
	Color curse = casting ? Color {229, 91, 108, 255} : Color {174, 132, 239, 255};
	int lift = casting ? 18 : 0;

	DrawLineEx({0.0f, static_cast<float>(height)}, {static_cast<float>(width * 0.31f), static_cast<float>(height - 96 - lift)}, 18.0f, Fade(hand, 0.32f));
	DrawLineEx({static_cast<float>(width * 0.31f), static_cast<float>(height - 96 - lift)}, {static_cast<float>(width * 0.40f), static_cast<float>(height - 178 - lift)}, 13.0f, hand);
	DrawLineEx({static_cast<float>(width * 0.31f), static_cast<float>(height - 96 - lift)}, {static_cast<float>(width * 0.46f), static_cast<float>(height - 122 - lift)}, 10.0f, hand);
	DrawLineEx({static_cast<float>(width * 0.40f), static_cast<float>(height - 178 - lift)}, {static_cast<float>(width * 0.47f), static_cast<float>(height - 216 - lift)}, 7.0f, hand);
	DrawCircleLines(static_cast<int>(width * 0.39f), height - 156 - lift, 25.0f, curse);

	DrawLineEx({static_cast<float>(width), static_cast<float>(height)}, {static_cast<float>(width * 0.69f), static_cast<float>(height - 96 - lift)}, 18.0f, Fade(hand, 0.32f));
	DrawLineEx({static_cast<float>(width * 0.69f), static_cast<float>(height - 96 - lift)}, {static_cast<float>(width * 0.60f), static_cast<float>(height - 178 - lift)}, 13.0f, hand);
	DrawLineEx({static_cast<float>(width * 0.69f), static_cast<float>(height - 96 - lift)}, {static_cast<float>(width * 0.54f), static_cast<float>(height - 122 - lift)}, 10.0f, hand);
	DrawLineEx({static_cast<float>(width * 0.60f), static_cast<float>(height - 178 - lift)}, {static_cast<float>(width * 0.53f), static_cast<float>(height - 216 - lift)}, 7.0f, hand);
	DrawCircleLines(static_cast<int>(width * 0.61f), height - 156 - lift, 25.0f, curse);
}
