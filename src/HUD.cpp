#include "HUD.h"

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <utility>

namespace {
void DrawMeter(int x, int y, int width, const char* label, float value, float maximum, Color color) {
	DrawText(label, x, y, 14, color);
	DrawRectangle(x, y + 19, width, 13, {3, 7, 10, 220});
	DrawRectangleLines(x, y + 19, width, 13, ColorAlpha(color, 0.72f));
	const float fraction = maximum > 0.0f ? std::clamp(value / maximum, 0.0f, 1.0f) : 0.0f;
	DrawRectangle(x + 2, y + 21, static_cast<int>((width - 4) * fraction), 9, color);
	DrawText(TextFormat("%.0f / %.0f", value, maximum), x + width + 9, y + 17, 13, color);
}

void DrawWitchHand(Vector2 wrist, bool mirrored, float raise, float recoil, Color ink) {
	const float side = mirrored ? -1.0f : 1.0f;
	wrist.y -= raise;
	wrist.x += side * recoil;
	const Vector2 palm {wrist.x + side * 34.0f, wrist.y - 68.0f};
	const Vector2 thumb {palm.x - side * 29.0f, palm.y - 14.0f};
	const Vector2 index {palm.x - side * 18.0f, palm.y - 62.0f};
	const Vector2 middle {palm.x + side * 1.0f, palm.y - 73.0f};
	const Vector2 ring {palm.x + side * 18.0f, palm.y - 65.0f};
	const Vector2 little {palm.x + side * 33.0f, palm.y - 48.0f};

	DrawTriangle({wrist.x - side * 25.0f, wrist.y + 25.0f}, {wrist.x + side * 27.0f, wrist.y + 25.0f},
		{palm.x + side * 19.0f, palm.y + 24.0f}, {4, 8, 12, 235});
	DrawLineEx({wrist.x - side * 22.0f, wrist.y + 28.0f}, {palm.x - side * 22.0f, palm.y + 15.0f}, 4.0f, ink);
	DrawLineEx({wrist.x + side * 27.0f, wrist.y + 28.0f}, {palm.x + side * 22.0f, palm.y + 16.0f}, 4.0f, ink);
	DrawLineEx({palm.x - side * 22.0f, palm.y + 16.0f}, thumb, 4.0f, ink);
	DrawLineEx(thumb, {thumb.x - side * 9.0f, thumb.y + 18.0f}, 3.0f, ink);
	DrawLineEx({palm.x - side * 17.0f, palm.y - 3.0f}, index, 4.0f, ink);
	DrawLineEx(index, {index.x + side * 5.0f, index.y + 22.0f}, 3.0f, ink);
	DrawLineEx({palm.x - side * 5.0f, palm.y - 9.0f}, middle, 4.0f, ink);
	DrawLineEx(middle, {middle.x + side * 1.0f, middle.y + 24.0f}, 3.0f, ink);
	DrawLineEx({palm.x + side * 7.0f, palm.y - 8.0f}, ring, 4.0f, ink);
	DrawLineEx(ring, {ring.x - side * 3.0f, ring.y + 23.0f}, 3.0f, ink);
	DrawLineEx({palm.x + side * 17.0f, palm.y - 1.0f}, little, 4.0f, ink);
	DrawLineEx(little, {little.x - side * 7.0f, little.y + 21.0f}, 3.0f, ink);
	DrawCircleLines(static_cast<int>(palm.x), static_cast<int>(palm.y + 7.0f), 9.0f, ColorAlpha(ink, 0.72f));
	DrawLine(static_cast<int>(palm.x - 6.0f), static_cast<int>(palm.y + 7.0f), static_cast<int>(palm.x + 6.0f), static_cast<int>(palm.y + 7.0f), ink);
}
}

void HUD::update(float dt) {
	noticeTimer_ = std::max(0.0f, noticeTimer_ - dt);
	killFlash_ = std::max(0.0f, killFlash_ - dt * 2.5f);
	if (noticeTimer_ <= 0.0f) {
		notice_.clear();
	}
}

void HUD::setNotice(std::string notice, float duration) {
	notice_ = std::move(notice);
	noticeTimer_ = duration;
}

void HUD::registerKill() {
	killFlash_ = 1.0f;
}

void HUD::draw(const Player& player, const CurseVector& spell, bool rummaging,
	int wave, int enemiesRemaining, int pickupsRemaining) const {
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	const Color healthColor {255, 77, 88, 255};
	const Color wardColor {83, 222, 232, 255};
	const Color textColor {218, 232, 225, 255};
	const Color dim {68, 116, 124, 255};

	DrawMeter(24, 20, 190, "FLESH", player.health(), player.maxHealth(), healthColor);
	DrawMeter(24, 60, 190, player.isWardActive() ? "WARD // RAISED" : "WARD // RMB or Q", player.ward(), player.maxWard(), wardColor);
	if (!player.isWardActive() && player.wardRegenDelay() > 0.0f) {
		DrawText(TextFormat("ward wakes in %.1fs", player.wardRegenDelay()), 24, 98, 12, dim);
	}

	const char* waveText = TextFormat("DISTRICT BREACH %i / 3", wave);
	DrawText(waveText, width - MeasureText(waveText, 18) - 24, 22, 18, {255, 174, 76, 255});
	const char* enemyText = TextFormat("HUNTERS %02i   LOOSE REAGENTS %02i", enemiesRemaining, pickupsRemaining);
	DrawText(enemyText, width - MeasureText(enemyText, 14) - 24, 48, 14, textColor);

	const int centerX = width / 2;
	const int centerY = height / 2;
	const int gap = player.hitMarker() > 0.0f ? 8 : 5;
	const Color reticle = player.hitMarker() > 0.0f ? WHITE : Color {255, 90, 103, 230};
	DrawLine(centerX - gap - 7, centerY, centerX - gap, centerY, reticle);
	DrawLine(centerX + gap, centerY, centerX + gap + 7, centerY, reticle);
	DrawLine(centerX, centerY - gap - 7, centerX, centerY - gap, reticle);
	DrawLine(centerX, centerY + gap, centerX, centerY + gap + 7, reticle);
	if (player.hitMarker() > 0.0f) {
		DrawLine(centerX - 9, centerY - 9, centerX - 3, centerY - 3, WHITE);
		DrawLine(centerX + 9, centerY - 9, centerX + 3, centerY - 3, WHITE);
		DrawLine(centerX - 9, centerY + 9, centerX - 3, centerY + 3, WHITE);
		DrawLine(centerX + 9, centerY + 9, centerX + 3, centerY + 3, WHITE);
	}

	if (!rummaging) {
		const int spellWidth = std::min(460, width - 48);
		const int spellX = centerX - spellWidth / 2;
		const int spellY = height - 91;
		DrawRectangle(spellX, spellY, spellWidth, 62, {3, 7, 11, 218});
		DrawRectangleLines(spellX, spellY, spellWidth, 62, {255, 85, 100, 205});
		DrawText(spell.name().c_str(), spellX + 13, spellY + 8, 18, textColor);
		DrawText(TextFormat("%s // %s", spell.baseName().c_str(), spell.ingredientSummary().c_str()), spellX + 13, spellY + 34, 13, dim);
		DrawText("LMB", spellX + spellWidth - 47, spellY + 21, 14, {255, 85, 100, 255});

		int totalReagents = 0;
		for (const int quantity : player.inventory()) totalReagents += quantity;
		const char* coatText = TextFormat("HOLD R // COAT %02i", totalReagents);
		DrawText(coatText, 24, height - 38, 15, textColor);
		if (spell.isFallback() && !player.hasAnyReagents()) {
			DrawText("POPPET NEVER RUNS DRY", 24, height - 61, 13, {255, 174, 76, 255});
		}
	}

	if (!notice_.empty()) {
		const int noticeWidth = MeasureText(notice_.c_str(), 18) + 28;
		DrawRectangle(centerX - noticeWidth / 2, 111, noticeWidth, 34, {4, 8, 12, 225});
		DrawRectangleLines(centerX - noticeWidth / 2, 111, noticeWidth, 34, killFlash_ > 0.0f ? healthColor : wardColor);
		DrawText(notice_.c_str(), centerX - MeasureText(notice_.c_str(), 18) / 2, 119, 18, textColor);
	}

	if (player.isWardActive()) {
		const float pulse = 10.0f + std::sin(static_cast<float>(GetTime()) * 8.0f) * 4.0f;
		DrawCircleLines(centerX, centerY, 94.0f + pulse, ColorAlpha(wardColor, 0.65f));
		DrawCircleLines(centerX, centerY, 116.0f - pulse * 0.4f, ColorAlpha(wardColor, 0.28f));
		for (int i = 0; i < 8; ++i) {
			const float angle = static_cast<float>(i) * PI / 4.0f;
			const Vector2 inner {centerX + std::cos(angle) * 104.0f, centerY + std::sin(angle) * 104.0f};
			const Vector2 outer {centerX + std::cos(angle) * 122.0f, centerY + std::sin(angle) * 122.0f};
			DrawLineV(inner, outer, wardColor);
		}
	}

	if (player.damageFlash() > 0.0f) {
		const unsigned char alpha = static_cast<unsigned char>(80.0f * player.damageFlash());
		const Color hurt {255, 23, 43, alpha};
		DrawRectangle(0, 0, width, 16, hurt);
		DrawRectangle(0, height - 16, width, 16, hurt);
		DrawRectangle(0, 0, 16, height, hurt);
		DrawRectangle(width - 16, 0, 16, height, hurt);
	}
}

void HUD::drawHands(const Player& player, bool rummaging) const {
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	const float castKick = std::sin(player.castAnimation() * PI) * 22.0f;
	const float bob = player.movementBob() * 260.0f;
	const float raise = rummaging ? 42.0f : (player.isWardActive() ? 90.0f : 0.0f);
	const Color ink = player.isWardActive() ? Color {93, 229, 238, 255} : Color {228, 230, 207, 255};
	const float leftX = rummaging ? width * 0.12f : (player.isWardActive() ? width * 0.36f : width * 0.24f);
	const float rightX = rummaging ? width * 0.88f : (player.isWardActive() ? width * 0.64f : width * 0.76f);
	DrawWitchHand({leftX, static_cast<float>(height) + bob}, false, raise, castKick, ink);
	DrawWitchHand({rightX, static_cast<float>(height) - bob}, true, raise, castKick, ink);
	if (player.isWardActive()) {
		const int centerX = width / 2;
		const int sigilY = height - 210;
		DrawCircleLines(centerX, sigilY, 25.0f, ColorAlpha(ink, 0.8f));
		DrawLine(centerX, sigilY - 20, centerX - 14, sigilY + 15, ink);
		DrawLine(centerX - 14, sigilY + 15, centerX + 17, sigilY + 3, ink);
		DrawLine(centerX + 17, sigilY + 3, centerX - 17, sigilY - 3, ink);
	}
}

void HUD::drawTerminalOverlay(bool victory, int kills) const {
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	DrawRectangle(0, 0, width, height, {2, 4, 7, 205});
	const Color color = victory ? Color {102, 234, 212, 255} : Color {255, 74, 89, 255};
	const char* title = victory ? "THE ROAD OUT OF SALEM OPENS" : "THE HUNTERS TAKE THE WITCH";
	const int size = width > 900 ? 40 : 28;
	DrawText(title, width / 2 - MeasureText(title, size) / 2, height / 2 - 82, size, color);
	const char* count = TextFormat("%i HUNTERS HEXED", kills);
	DrawText(count, width / 2 - MeasureText(count, 20) / 2, height / 2 - 22, 20, {218, 232, 225, 255});
	const char* prompt = "PRESS ENTER TO BEGIN ANOTHER BREACH";
	DrawText(prompt, width / 2 - MeasureText(prompt, 18) / 2, height / 2 + 30, 18, color);
}
