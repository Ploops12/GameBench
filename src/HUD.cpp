#include <algorithm>
#include <cmath>
#include "Game.h"

namespace {
static void drawMeter(int x, int y, int width, int height, float value, float maxValue, Color fill, const char* label) {
	DrawRectangle(x, y, width, height, Fade(BLACK, 0.75f));
	DrawRectangleLines(x, y, width, height, LIGHTGRAY);

	float ratio = maxValue > 0.0f ? std::clamp(value / maxValue, 0.0f, 1.0f) : 0.0f;
	DrawRectangle(x + 3, y + 3, static_cast<int>((width - 6) * ratio), height - 6, fill);
	DrawText(label, x, y - 20, 18, RAYWHITE);
	DrawText(TextFormat("%d / %d", static_cast<int>(value), static_cast<int>(maxValue)), x + width + 10, y + 2, 18, WHITE);
}

static void drawHandOverlay(const Player& player, float timeSeconds) {
	const int w = GetScreenWidth();
	const int h = GetScreenHeight();
	float bob = std::sin(timeSeconds * 6.0f) * 6.0f;
	float wardGlow = player.isWarding() ? 1.0f : 0.0f;
	float flash = player.getCastFlash();

	Color lineColor = ColorAlpha(WHITE, 0.85f);
	Color sigilColor = ColorAlpha(player.isWarding() ? SKYBLUE : ORANGE, 0.5f + 0.35f * flash);

	Vector2 leftPalm {w * 0.28f, h * 0.82f + bob};
	Vector2 rightPalm {w * 0.72f, h * 0.82f - bob};

	DrawCircleLinesV(leftPalm, 34.0f, lineColor);
	DrawCircleLinesV(rightPalm, 34.0f, lineColor);

	for (int i = 0; i < 4; ++i) {
		float bend = static_cast<float>(i - 1) * 10.0f;
		DrawLineEx(leftPalm, {leftPalm.x - 36.0f + bend, leftPalm.y - 80.0f - bend * 0.4f}, 3.0f, lineColor);
		DrawLineEx(rightPalm, {rightPalm.x + 36.0f - bend, rightPalm.y - 80.0f + bend * 0.4f}, 3.0f, lineColor);
	}

	DrawLineEx({leftPalm.x - 20.0f, leftPalm.y + 30.0f}, {leftPalm.x - 90.0f, leftPalm.y + 90.0f}, 5.0f, lineColor);
	DrawLineEx({rightPalm.x + 20.0f, rightPalm.y + 30.0f}, {rightPalm.x + 90.0f, rightPalm.y + 90.0f}, 5.0f, lineColor);

	DrawCircleLinesV(leftPalm, 18.0f + 14.0f * flash + 10.0f * wardGlow, sigilColor);
	DrawCircleLinesV(rightPalm, 18.0f + 14.0f * flash + 10.0f * wardGlow, sigilColor);
	DrawLineEx({leftPalm.x - 12.0f, leftPalm.y}, {leftPalm.x + 12.0f, leftPalm.y}, 2.0f, sigilColor);
	DrawLineEx({leftPalm.x, leftPalm.y - 12.0f}, {leftPalm.x, leftPalm.y + 12.0f}, 2.0f, sigilColor);
	DrawLineEx({rightPalm.x - 12.0f, rightPalm.y}, {rightPalm.x + 12.0f, rightPalm.y}, 2.0f, sigilColor);
	DrawLineEx({rightPalm.x, rightPalm.y - 12.0f}, {rightPalm.x, rightPalm.y + 12.0f}, 2.0f, sigilColor);
}
}

void HUD::draw(const Game& game) const {
	const Player& player = game.getPlayer();
	const CoatMenu& coatMenu = game.getCoatMenu();
	const int screenW = GetScreenWidth();

	drawMeter(24, 24, 220, 22, player.getHealth(), player.getMaxHealth(), {201, 71, 71, 255}, "HEALTH");
	drawMeter(24, 74, 220, 22, player.getWard(), player.getMaxWard(), {113, 201, 255, 255}, "WARD");

	DrawText(TextFormat("Wave %d", game.getWaveIndex()), 24, 114, 20, RAYWHITE);
	DrawText(TextFormat("Enemies %d", static_cast<int>(game.getEnemies().size())), 24, 138, 20, RAYWHITE);
	DrawText("LMB cast  RMB ward  TAB rummage  Shift sprint", 24, 164, 18, GRAY);
	DrawText(player.isWarding() ? "Ward active" : (player.getWardCooldown() > 0.0f ? TextFormat("Ward cracked %.1fs", player.getWardCooldown()) : "Ward ready"), 24, 188, 18, player.isWarding() ? SKYBLUE : (player.getWardCooldown() > 0.0f ? ORANGE : GREEN));

	ComposedSpell preview = coatMenu.getPreview();
	Rectangle previewBox {static_cast<float>(screenW - 340), 24.0f, 316.0f, 112.0f};
	DrawRectangleRounded(previewBox, 0.06f, 8, Fade(BLACK, 0.74f));
	DrawRectangleRoundedLinesEx(previewBox, 0.06f, 8, 2.0f, LIGHTGRAY);
	DrawText(preview.label.c_str(), static_cast<int>(previewBox.x + 14.0f), static_cast<int>(previewBox.y + 14.0f), 24, getBaseVectorColor(preview.base));
	DrawText(preview.preview.c_str(), static_cast<int>(previewBox.x + 14.0f), static_cast<int>(previewBox.y + 46.0f), 18, WHITE);
	Color statusColor = player.isRummaging() ? Color {255, 170, 170, 255} : Color {180, 255, 180, 255};
	DrawText(player.isRummaging() ? "Rummaging: slow, no sprint" : "Ready", static_cast<int>(previewBox.x + 14.0f), static_cast<int>(previewBox.y + 76.0f), 18, statusColor);

	const ReagentInventory& inventory = player.getInventory();
	int rowY = 210;
	for (const ReagentInfo& info : getReagentInfos()) {
		const int index = getReagentIndex(info.type);
		DrawText(TextFormat("%c %s  x%d", static_cast<char>(info.keybind), info.shortName, inventory[index]), 24, rowY, 18, info.color);
		rowY += 22;
	}

	int selectedY = 364;
	DrawText("Loaded mix", 24, selectedY, 18, LIGHTGRAY);
	for (int slot = 0; slot < coatMenu.getRecipe().slotCount; ++slot) {
		const ReagentInfo& info = getReagentInfo(coatMenu.getRecipe().slots[slot]);
		DrawText(TextFormat("%d. %s", slot + 1, info.name), 24, selectedY + 24 + slot * 20, 18, info.color);
	}

	int centerX = GetScreenWidth() / 2;
	int centerY = GetScreenHeight() / 2;
	Color reticle = player.isWarding() ? SKYBLUE : ORANGE;
	DrawCircleLines(centerX, centerY, 12.0f + player.getCastFlash() * 10.0f, reticle);
	DrawLine(centerX - 16, centerY, centerX + 16, centerY, reticle);
	DrawLine(centerX, centerY - 16, centerX, centerY + 16, reticle);

	drawHandOverlay(player, game.getTimeSeconds());
	coatMenu.draw(player.getInventory());
}
