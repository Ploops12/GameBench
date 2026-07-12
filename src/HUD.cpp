#include "HUD.h"

#include <algorithm>
#include <array>
#include <cmath>

#include <raylib.h>

namespace {
Color reagentColor(ReagentType type) {
	switch (type) {
	case ReagentType::GraveSalt:
		return Color {196, 240, 231, 255};
	case ReagentType::ChurchGlass:
		return Color {74, 207, 225, 255};
	case ReagentType::WidowNettle:
		return Color {139, 239, 100, 255};
	case ReagentType::PlagueHoney:
		return Color {244, 190, 66, 255};
	case ReagentType::MothDust:
		return Color {195, 128, 238, 255};
	case ReagentType::BlackWax:
		return Color {241, 92, 115, 255};
	case ReagentType::Count:
		break;
	}

	return RAYWHITE;
}

void drawMeter(Rectangle bounds, float fraction, Color color, const char* label, const char* value) {
	fraction = std::clamp(fraction, 0.0f, 1.0f);
	DrawRectangleRec(bounds, Color {7, 13, 22, 205});
	DrawRectangleLinesEx(bounds, 1.4f, color);
	DrawRectangle(static_cast<int>(bounds.x + 3.0f), static_cast<int>(bounds.y + 3.0f),
		static_cast<int>((bounds.width - 6.0f) * fraction), static_cast<int>(bounds.height - 6.0f),
		Color {color.r, color.g, color.b, 155});
	DrawText(label, static_cast<int>(bounds.x), static_cast<int>(bounds.y - 19.0f), 14, color);
	DrawText(value, static_cast<int>(bounds.x + bounds.width + 7.0f), static_cast<int>(bounds.y + 2.0f), 13, RAYWHITE);
}

void drawFallbackHands(int screenWidth, int screenHeight, Color color) {
	const float width = static_cast<float>(screenWidth);
	const Vector2 leftWrist {width * 0.35f, static_cast<float>(screenHeight - 24)};
	const Vector2 rightWrist {width * 0.65f, static_cast<float>(screenHeight - 24)};
	const Vector2 leftPalm {width * 0.42f, static_cast<float>(screenHeight - 132)};
	const Vector2 rightPalm {width * 0.58f, static_cast<float>(screenHeight - 132)};
	DrawLineEx(leftWrist, leftPalm, 8.0f, color);
	DrawLineEx(rightWrist, rightPalm, 8.0f, color);
	for (int finger = 0; finger < 4; ++finger) {
		const float fingerIndex = static_cast<float>(finger);
		DrawLineEx(leftPalm, Vector2 {leftPalm.x - 28.0f + fingerIndex * 17.0f, leftPalm.y - 48.0f - fingerIndex * 5.0f}, 4.0f, color);
		DrawLineEx(rightPalm, Vector2 {rightPalm.x - 23.0f + fingerIndex * 17.0f, rightPalm.y - 53.0f + fingerIndex * 5.0f}, 4.0f, color);
	}
}
}

void HUD::draw(const Player& player, const CoatMenu& coatMenu, const ReagentInventory& inventory,
	const Sprite* hands, int livingEnemies, const std::string& notification, float notificationTime) const {
	const int screenWidth = GetScreenWidth();
	const int screenHeight = GetScreenHeight();
	const Color healthColor {250, 99, 105, 255};
	const Color wardColor {94, 223, 238, 255};
	const Color ink {226, 244, 240, 255};
	const Color dim {162, 190, 193, 255};

	drawMeter(Rectangle {26.0f, 42.0f, 174.0f, 17.0f}, player.getHealth() / player.getMaximumHealth(),
		healthColor, "HEALTH", TextFormat("%.0f", player.getHealth()));
	drawMeter(Rectangle {26.0f, 89.0f, 174.0f, 17.0f}, player.getWard() / player.getMaximumWard(),
		wardColor, player.isWardActive() ? "WARD // RAISED" : "WARD", TextFormat("%.0f", player.getWard()));

	DrawRectangleRec(Rectangle {static_cast<float>(screenWidth - 180), 28.0f, 154.0f, 99.0f}, Color {7, 13, 22, 205});
	DrawRectangleLinesEx(Rectangle {static_cast<float>(screenWidth - 180), 28.0f, 154.0f, 99.0f}, 1.2f, dim);
	DrawText("SATCHEL", screenWidth - 165, 40, 15, ink);
	static constexpr std::array<const char*, kReagentTypeCount> kShortNames {"SALT", "GLASS", "NETTLE", "HONEY", "MOTH", "WAX"};
	for (std::size_t index = 0; index < kReagentTypeCount; ++index) {
		const int column = static_cast<int>(index % 3);
		const int row = static_cast<int>(index / 3);
		const int x = screenWidth - 164 + column * 47;
		const int y = 65 + row * 29;
		const ReagentType type = static_cast<ReagentType>(index);
		DrawCircleLines(x + 7, y + 7, 6.0f, reagentColor(type));
		DrawText(TextFormat("%d", inventory.count(type)), x + 17, y, 15, ink);
		DrawText(kShortNames[index], x, y + 14, 9, dim);
	}

	const Color handColor = player.isWardActive() ? wardColor : Color {235, 231, 211, 255};
	if (hands != nullptr && hands->loaded()) {
		const Rectangle destination {static_cast<float>(screenWidth / 2 - 210), static_cast<float>(screenHeight - 250), 420.0f, 275.0f};
		hands->drawFramePro(player.isWardActive() ? 5 : 0, destination, {}, 0.0f, handColor);
	} else {
		drawFallbackHands(screenWidth, screenHeight, handColor);
	}

	const CurseVector& prepared = coatMenu.getCurseVector();
	const SpellStats spell = prepared.stats();
	const int hotbarY = screenHeight - 107;
	DrawRectangleRec(Rectangle {static_cast<float>(screenWidth / 2 - 235), static_cast<float>(hotbarY), 470.0f, 73.0f},
		Color {7, 13, 22, 201});
	DrawRectangleLinesEx(Rectangle {static_cast<float>(screenWidth / 2 - 235), static_cast<float>(hotbarY), 470.0f, 73.0f}, 1.3f,
		prepared.canConsume(inventory) ? Color {125, 240, 204, 255} : Color {249, 136, 110, 255});
	DrawText(TextFormat("READY: %s", curseVectorName(prepared.base()).data()), screenWidth / 2 - 218, hotbarY + 12, 18, ink);
	DrawText(prepared.reagentSummary().c_str(), screenWidth / 2 - 218, hotbarY + 34, 14, dim);
	if (prepared.base() == CurseVectorType::Apple) {
		const SelfHexStats self = selfHexStats(spell);
		DrawText(TextFormat("VIT +%.0f  WARD +%.0f", self.healthRestore, self.wardGain),
			screenWidth / 2 + 37, hotbarY + 13, 14, ink);
	} else {
		DrawText(TextFormat("DMG %.0f  R %.1f  WARD +%.0f", spell.damage, spell.radius, spell.wardGain),
			screenWidth / 2 + 37, hotbarY + 13, 14, ink);
	}
	DrawText("LMB CAST", screenWidth / 2 + 128, hotbarY + 39, 14, Color {126, 239, 204, 255});

	const int centreX = screenWidth / 2;
	const int centreY = screenHeight / 2;
	const Color reticle = player.isWardActive() ? wardColor : ink;
	DrawCircleLines(centreX, centreY, player.isWardActive() ? 16.0f : 9.0f, reticle);
	DrawLine(centreX - 15, centreY, centreX - 5, centreY, reticle);
	DrawLine(centreX + 5, centreY, centreX + 15, centreY, reticle);
	DrawLine(centreX, centreY - 15, centreX, centreY - 5, reticle);
	DrawLine(centreX, centreY + 5, centreX, centreY + 15, reticle);
	if (player.isWardActive()) {
		const float time = static_cast<float>(GetTime());
		DrawCircleLines(centreX, centreY, 57.0f + sinf(time * 7.0f) * 4.0f, Color {wardColor.r, wardColor.g, wardColor.b, 165});
	}

	DrawText(TextFormat("HOSTILES %d", livingEnemies), 26, screenHeight - 33, 15,
		livingEnemies > 0 ? Color {255, 148, 116, 255} : Color {126, 239, 204, 255});
	if (!coatMenu.isOpen()) {
		DrawText("[R] HOLD COAT  [TAB] TOGGLE   [F/RMB] WARD   [ESC] MOUSE", screenWidth / 2 - 232,
			screenHeight - 18, 13, dim);
	}
	if (notificationTime > 0.0f && !notification.empty()) {
		const int width = MeasureText(notification.c_str(), 19);
		DrawRectangle(screenWidth / 2 - width / 2 - 10, 138, width + 20, 31, Color {4, 10, 17, 218});
		DrawRectangleLines(screenWidth / 2 - width / 2 - 10, 138, width + 20, 31, Color {125, 240, 204, 255});
		DrawText(notification.c_str(), screenWidth / 2 - width / 2, 144, 19, ink);
	}
	if (player.getThornGuardTime() > 0.0f || player.getMendTime() > 0.0f ||
		player.getPanicStepTime() > 0.0f || player.getWaxShellTime() > 0.0f) {
		DrawText(TextFormat("SELF HEX  THORN %.1f  MEND %.1f  HASTE %.1f  WAX %.1f",
			player.getThornGuardTime(), player.getMendTime(), player.getPanicStepTime(), player.getWaxShellTime()),
			26, 122, 13, Color {176, 231, 189, 255});
	}

	if (player.getDamageFlash() > 0.0f) {
		const unsigned char alpha = static_cast<unsigned char>(player.getDamageFlash() / 0.28f * 76.0f);
		DrawRectangle(0, 0, screenWidth, screenHeight, Color {255, 59, 69, alpha});
	}
}
