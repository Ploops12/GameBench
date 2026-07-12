#include "HUD.h"

#include "CurseVector.hxx"
#include "Player.hxx"

#include <algorithm>

namespace {
	constexpr Color LINE{224, 230, 214, 255};
	constexpr Color HEALTH{224, 80, 92, 255};
	constexpr Color WARD{102, 206, 221, 255};
	constexpr Color ACCENT{224, 154, 90, 255};
	constexpr Color PANEL{8, 11, 19, 208};
}

void HUD::draw(const Player& player, const CurseVector& spell, bool rummaging, bool castReady, const std::string& status) const {
	const int screenWidth = GetScreenWidth();
	const int screenHeight = GetScreenHeight();

	DrawRectangle(18, 18, 270, 142, PANEL);
	DrawRectangleLinesEx({18.0f, 18.0f, 270.0f, 142.0f}, 2.0f, ColorAlpha(LINE, 0.8f));
	DrawText("VITALS", 34, 31, 14, ColorAlpha(LINE, 0.7f));
	DrawText(TextFormat("HEALTH %03d", static_cast<int>(player.health())), 34, 52, 22, HEALTH);
	DrawRectangle(34, 82, 238, 13, ColorAlpha(HEALTH, 0.18f));
	DrawRectangle(34, 82, static_cast<int>(238.0f * std::clamp(player.health() / player.maxHealth(), 0.0f, 1.0f)), 13, HEALTH);
	DrawRectangleLines(34, 82, 238, 13, ColorAlpha(LINE, 0.6f));
	DrawText(TextFormat("WARD %03d / %03d", static_cast<int>(player.ward()), static_cast<int>(player.maxWard())), 34, 106, 16, WARD);
	DrawRectangle(34, 130, 238, 10, ColorAlpha(WARD, 0.15f));
	DrawRectangle(34, 130, static_cast<int>(238.0f * std::clamp(player.ward() / player.maxWard(), 0.0f, 1.0f)), 10, WARD);
	if (player.wardCooldown() > 0.0f) DrawText(TextFormat("LOCKOUT %.1fs", player.wardCooldown()), 178, 106, 11, ColorAlpha(ACCENT, 0.9f));
	else if (player.ward() < player.maxWard() && !player.wardActive()) DrawText("RECHARGING", 178, 119, 11, ColorAlpha(WARD, 0.8f));

	const Rectangle rightPanel{screenWidth - 286.0f, 18.0f, 268.0f, 146.0f};
	DrawRectangleRec(rightPanel, PANEL);
	DrawRectangleLinesEx(rightPanel, 2.0f, ColorAlpha(LINE, 0.8f));
	DrawText("COAT RUMMAGE", static_cast<int>(rightPanel.x + 16.0f), static_cast<int>(rightPanel.y + 14.0f), 18, ACCENT);
	DrawText("hold R / toggle TAB", static_cast<int>(rightPanel.x + 16.0f), static_cast<int>(rightPanel.y + 42.0f), 15, LINE);
	DrawText("F hold  WARD SHIELD", static_cast<int>(rightPanel.x + 16.0f), static_cast<int>(rightPanel.y + 66.0f), 14, player.wardActive() ? WARD : ColorAlpha(LINE, 0.75f));
	DrawText("LMB cast   1/2/3 vector", static_cast<int>(rightPanel.x + 16.0f), static_cast<int>(rightPanel.y + 88.0f), 14, ColorAlpha(LINE, 0.8f));
	DrawText("WASD move  SHIFT sprint  SPACE jump", static_cast<int>(rightPanel.x + 16.0f), static_cast<int>(rightPanel.y + 110.0f), 12, ColorAlpha(LINE, 0.65f));

	const Rectangle spellPanel{screenWidth / 2.0f - 230.0f, static_cast<float>(screenHeight - 123), 460.0f, 88.0f};
	DrawRectangleRec(spellPanel, PANEL);
	DrawRectangleLinesEx(spellPanel, 2.0f, spell.base() == BaseVector::Poppet ? ColorAlpha(LINE, 0.8f) : ACCENT);
	DrawText(spell.name().c_str(), static_cast<int>(spellPanel.x + 18.0f), static_cast<int>(spellPanel.y + 13.0f), 18, spell.base() == BaseVector::Poppet ? LINE : ACCENT);
	DrawText(spell.deliveryText().c_str(), static_cast<int>(spellPanel.x + 18.0f), static_cast<int>(spellPanel.y + 39.0f), 13, ColorAlpha(LINE, 0.72f));
	DrawText(spell.outcomeText().c_str(), static_cast<int>(spellPanel.x + 18.0f), static_cast<int>(spellPanel.y + 59.0f), 13, ColorAlpha(LINE, 0.9f));
	DrawText(castReady ? "READY" : "RECITING", static_cast<int>(spellPanel.x + spellPanel.width - 82.0f), static_cast<int>(spellPanel.y + 15.0f), 12, castReady ? WARD : ColorAlpha(ACCENT, 0.8f));

	const int reagentY = screenHeight - 114;
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		const ReagentType type = ReagentFromIndex(i);
		const int x = 22 + i * 76;
		DrawRectangle(x, reagentY, 68, 66, ColorAlpha(PANEL, 0.9f));
		DrawRectangleLines(x, reagentY, 68, 66, ColorAlpha(GetReagentDefinition(type).color, 0.75f));
		DrawReagentIcon(type, {static_cast<float>(x + 23), static_cast<float>(reagentY + 27)}, 29.0f);
		DrawText(TextFormat("%d", player.reagentQuantity(type)), x + 46, reagentY + 17, 17, LINE);
		DrawText(GetReagentDefinition(type).shortName, x + 5, reagentY + 48, 10, ColorAlpha(LINE, 0.78f));
	}

	if (!status.empty()) {
		const int textWidth = MeasureText(status.c_str(), 18);
		DrawRectangle(screenWidth / 2 - textWidth / 2 - 16, 22, textWidth + 32, 30, ColorAlpha(PANEL, 0.88f));
		DrawText(status.c_str(), screenWidth / 2 - textWidth / 2, 28, 18, LINE);
	}

	if (rummaging) {
		DrawText("COAT OPEN // movement slowed // keep moving", screenWidth / 2 - 206, 60, 15, ACCENT);
	}

	drawCrosshair(player.wardActive());
}

void HUD::drawCrosshair(bool wardActive) const {
	const int x = GetScreenWidth() / 2;
	const int y = GetScreenHeight() / 2;
	const Color color = wardActive ? WARD : LINE;
	DrawLine(x - 13, y, x - 4, y, color);
	DrawLine(x + 4, y, x + 13, y, color);
	DrawLine(x, y - 13, x, y - 4, color);
	DrawLine(x, y + 4, x, y + 13, color);
	DrawCircleLines(x, y, 3.0f, color);
}
