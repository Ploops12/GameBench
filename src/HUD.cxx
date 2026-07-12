#include "HUD.h"

#include "Player.hxx"

namespace {
	const char* AssetPath(const char* relative) {
		static char path[512];
		if (FileExists(relative)) return relative;
		TextCopy(path, TextFormat("../%s", relative));
		return path;
	}

}

HUD::HUD() {
	hands = LoadTexture(AssetPath("resources/generated/hands/witch_hands_sheet.png"));
	handsLoaded = hands.id != 0;
}

HUD::~HUD() {
	if (handsLoaded) UnloadTexture(hands);
}

void HUD::Draw(const Player& player, const CurseVector& curse, const std::array<int, REAGENT_COUNT>& inventory,
	bool coatOpen, int kills, const std::string& message, float messageTimer) const {
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	const Color panel = Fade(Color{5, 11, 17, 255}, 0.86f);
	DrawRectangle(18, 18, 230, 152, panel);
	DrawRectangleLinesEx({18.0f, 18.0f, 230.0f, 152.0f}, 2.0f, BEIGE);
	DrawText("VITALS", 32, 29, 17, BEIGE);
	DrawText("HEALTH", 32, 56, 13, LIGHTGRAY);
	DrawRectangle(32, 75, 190, 14, DARKGRAY);
	DrawRectangle(32, 75, static_cast<int>(190.0f * player.GetHealth() / player.GetMaxHealth()), 14, RED);
	DrawText(TextFormat("%03.0f / %03.0f", player.GetHealth(), player.GetMaxHealth()), 32, 94, 15, WHITE);
	DrawText("WARD", 32, 120, 13, LIGHTGRAY);
	DrawRectangle(32, 138, 190, 14, DARKGRAY);
	DrawRectangle(32, 138, static_cast<int>(190.0f * player.GetWard() / player.GetMaxWard()), 14, SKYBLUE);

	DrawRectangle(width - 230, 18, 212, 124, panel);
	DrawRectangleLinesEx({static_cast<float>(width - 230), 18.0f, 212.0f, 124.0f}, 2.0f, BEIGE);
	DrawText("SATCHEL", width - 214, 30, 17, BEIGE);
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		const int x = width - 214 + (i % 3) * 62;
		const int y = 58 + (i / 3) * 36;
		DrawCircle(x + 9, y + 8, 7.0f, Fade(ReagentColor(static_cast<ReagentType>(i)), 0.9f));
		DrawText(TextFormat("%s:%d", ReagentKey(static_cast<ReagentType>(i)), inventory[i]), x + 19, y, 12, LIGHTGRAY);
	}
	DrawText(TextFormat("KILLS %02d", kills), width - 214, 124, 12, LIGHTGRAY);

	const int centerX = width / 2;
	const int centerY = height / 2;
	DrawLine(centerX - 10, centerY, centerX + 10, centerY, Fade(WHITE, 0.85f));
	DrawLine(centerX, centerY - 10, centerX, centerY + 10, Fade(WHITE, 0.85f));
	DrawCircleLines(centerX, centerY, 16.0f, Fade(ORANGE, 0.75f));

	const int bottomY = height - 106;
	DrawRectangle(width / 2 - 245, bottomY, 490, 84, panel);
	DrawRectangleLinesEx({width / 2.0f - 245.0f, static_cast<float>(bottomY), 490.0f, 84.0f}, 2.0f, ORANGE);
	DrawText("ACTIVE HEX", width / 2 - 225, bottomY + 10, 13, ORANGE);
	DrawText(curse.GetName().c_str(), width / 2 - 225, bottomY + 31, 15, WHITE);
	const SpellStats stats = curse.CalculateStats();
	DrawText(TextFormat("DMG %.0f  SPL %.1f  SNARE %.1f  %s", stats.damage, stats.splashRadius, stats.snareSeconds, stats.seeks ? "SEEK" : "DIRECT"), width / 2 - 225, bottomY + 56, 13, LIGHTGRAY);

	DrawText(coatOpen ? "COAT OPEN // MOVE SLOW // VULNERABLE" : "R COAT RUMMAGE    F WARD    LMB CAST    SHIFT SPRINT", 24, height - 25, 14, coatOpen ? Color{255, 126, 110, 255} : LIGHTGRAY);
	if (messageTimer > 0.0f) {
		const int messageWidth = MeasureText(message.c_str(), 20);
		DrawRectangle(centerX - messageWidth / 2 - 14, 166, messageWidth + 28, 32, Fade(Color{8, 12, 17, 255}, 0.92f));
		DrawText(message.c_str(), centerX - messageWidth / 2, 172, 20, ORANGE);
	}
	player.DrawWardOverlay();
}

void HUD::DrawHands(bool wardActive, bool casting) const {
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	if (handsLoaded) {
		const Rectangle source{0.0f, casting ? 328.0f : 0.0f, 192.0f, 328.0f};
		const Rectangle destination{width / 2.0f - 172.0f, height - 310.0f, 344.0f, 590.0f};
		DrawTexturePro(hands, source, destination, {0.0f, 0.0f}, 0.0f, wardActive ? SKYBLUE : WHITE);
	} else {
		const Color color = wardActive ? SKYBLUE : BEIGE;
		DrawCircleLines(width / 2 - 95, height - 70, 60.0f, color);
		DrawCircleLines(width / 2 + 95, height - 70, 60.0f, color);
	}
}
