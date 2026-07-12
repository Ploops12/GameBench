#include "HUD.h"

#include <algorithm>
#include <string>

#include <raylib.h>

void HUD::draw(const Player& player, const CurseVector& spell, bool coatOpen,
	const char* announcement, float announcementTime) const {
	const int screenWidth = GetScreenWidth();
	const int screenHeight = GetScreenHeight();
	const Color cyan {134, 232, 255, 255};
	const Color rose {255, 106, 106, 255};
	const Color wardColor {132, 220, 255, 255};

	drawMeter(24, 26, 226, "HEALTH", player.getHealth(), player.getMaxHealth(), rose);
	drawMeter(24, 82, 226, player.isWardActive() ? "WARD: RAISED" : "WARD: READY", player.getWard(), player.getMaxWard(), wardColor);
	DrawText("RMB hold shield  |  drains while raised, rebuilds while lowered", 24, 126, 12, Fade(LIGHTGRAY, 0.85f));

	const Rectangle activeSpell {24.0f, static_cast<float>(screenHeight - 138), 360.0f, 92.0f};
	DrawRectangleRec(activeSpell, Fade({10, 18, 30, 255}, 0.84f));
	DrawRectangleLinesEx(activeSpell, 1.5f, spell.getStats().color);
	DrawText("ACTIVE CURSE", 36, screenHeight - 127, 13, cyan);
	DrawText(spell.getDisplayName().c_str(), 36, screenHeight - 104, 20, spell.getStats().color);
	DrawText(CurseVector::getDeliveryName(spell.getBase()), 36, screenHeight - 77, 13, LIGHTGRAY);
	DrawText("LMB cast", 36, screenHeight - 57, 13, RAYWHITE);

	const std::array<int, ReagentCount>& inventory = player.getInventory();
	const Rectangle satchel {static_cast<float>(screenWidth - 243), 26.0f, 219.0f, 176.0f};
	DrawRectangleRec(satchel, Fade({10, 18, 30, 255}, 0.84f));
	DrawRectangleLinesEx(satchel, 1.5f, cyan);
	DrawText("COAT STOCK", screenWidth - 231, 38, 14, cyan);
	for (std::size_t index = 0; index < ReagentCount; ++index) {
		const ReagentDefinition& definition = getReagentDefinition(reagentFromIndex(index));
		const int row = static_cast<int>(index / 2U);
		const int column = static_cast<int>(index % 2U);
		const int x = screenWidth - 231 + column * 105;
		const int y = 63 + row * 42;
		DrawRectangleLines(x, y, 94, 31, Fade(definition.color, inventory[index] > 0 ? 0.85f : 0.35f));
		DrawText(definition.shortName, x + 6, y + 8, 12, definition.color);
		DrawText(("x" + std::to_string(inventory[index])).c_str(), x + 61, y + 7, 14, inventory[index] > 0 ? RAYWHITE : GRAY);
	}

	const int centerX = screenWidth / 2;
	const int centerY = screenHeight / 2;
	const Color reticle = player.isWardActive() ? wardColor : spell.getStats().color;
	DrawCircleLines(centerX, centerY, 9.0f, reticle);
	DrawLine(centerX - 15, centerY, centerX - 4, centerY, reticle);
	DrawLine(centerX + 4, centerY, centerX + 15, centerY, reticle);
	DrawLine(centerX, centerY - 15, centerX, centerY - 4, reticle);
	DrawLine(centerX, centerY + 4, centerX, centerY + 15, reticle);

	if (!coatOpen) {
		DrawText("HOLD [R]  COAT RUMMAGE", screenWidth - 246, screenHeight - 39, 15, cyan);
	} else {
		DrawText("RUMMAGING: HUNT CONTINUES", 24, 153, 14, {255, 186, 114, 255});
	}
	DrawText("WASD move  |  SHIFT sprint  |  SPACE jump  |  ESC mouse", screenWidth / 2 - 214, screenHeight - 25, 13, Fade(LIGHTGRAY, 0.85f));

	if (announcementTime > 0.0f && announcement != nullptr) {
		const int width = MeasureText(announcement, 19);
		DrawRectangle(screenWidth / 2 - width / 2 - 12, 28, width + 24, 31, Fade({11, 17, 29, 255}, 0.9f));
		DrawRectangleLines(screenWidth / 2 - width / 2 - 12, 28, width + 24, 31, spell.getStats().color);
		DrawText(announcement, screenWidth / 2 - width / 2, 35, 19, RAYWHITE);
	}

	if (player.getDamageFlash() > 0.0f) {
		DrawRectangleLinesEx({4.0f, 4.0f, static_cast<float>(screenWidth - 8), static_cast<float>(screenHeight - 8)}, 4.0f,
			Fade(rose, std::min(1.0f, player.getDamageFlash() * 3.0f)));
	}
}

void HUD::drawMeter(int x, int y, int width, const char* label, float value, float maximum, Color color) const {
	const float ratio = maximum > 0.0f ? std::clamp(value / maximum, 0.0f, 1.0f) : 0.0f;
	DrawText(label, x, y, 14, color);
	DrawRectangle(x, y + 21, width, 15, Fade({20, 27, 42, 255}, 0.95f));
	DrawRectangle(x, y + 21, static_cast<int>(static_cast<float>(width) * ratio), 15, Fade(color, 0.75f));
	DrawRectangleLines(x, y + 21, width, 15, RAYWHITE);
	DrawText((std::to_string(static_cast<int>(value)) + " / " + std::to_string(static_cast<int>(maximum))).c_str(), x + width + 9, y + 19, 14, RAYWHITE);
}
