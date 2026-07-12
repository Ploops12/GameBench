#include "CoatMenu.h"
#include "Player.hxx"
#include <raylib.h>
#include <string>

namespace {
void outlinedPanel(Rectangle rect, Color color) {
	DrawRectangleRec(rect, Color{8, 10, 12, 225});
	DrawRectangleLinesEx(rect, 2.0f, color);
	DrawLine(static_cast<int>(rect.x + 8), static_cast<int>(rect.y + 8),
		static_cast<int>(rect.x + rect.width - 8), static_cast<int>(rect.y + 8), Fade(color, 0.5f));
}
}

void CoatMenu::update(Player& player) {
	if (!isOpen()) return;
	if (IsKeyPressed(KEY_ONE)) {
		player.getSpell().setBase(BaseVector::Bottle);
		player.getSpell().clearReagents();
	}
	if (IsKeyPressed(KEY_TWO)) {
		player.getSpell().setBase(BaseVector::Apple);
		player.getSpell().clearReagents();
	}
	if (IsKeyPressed(KEY_THREE)) {
		player.getSpell().setBase(BaseVector::Spider);
		player.getSpell().clearReagents();
	}
	if (IsKeyPressed(KEY_FOUR)) {
		player.getSpell().setBase(BaseVector::Poppet);
		player.getSpell().clearReagents();
	}

	const int keys[REAGENT_COUNT] = {KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N};
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		if (IsKeyPressed(keys[i])) player.selectReagent(static_cast<ReagentType>(i));
	}
	if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE)) player.getSpell().clearReagents();
}

void CoatMenu::draw(const Player& player) const {
	if (!isOpen()) return;
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	const Color line{190, 215, 190, 255};
	const Rectangle panel{24.0f, static_cast<float>(height - 270), static_cast<float>(width - 48), 246.0f};
	outlinedPanel(panel, line);
	DrawText("COAT OPEN // YOU ARE SLOW, EXPOSED, AND CANNOT SPRINT OR JUMP",
		static_cast<int>(panel.x + 18), static_cast<int>(panel.y + 16), 18, Color{245, 105, 90, 255});

	const char* bases[4] = {"[1] BOTTLE / THROW", "[2] APPLE / EAT", "[3] SPIDER / MINE", "[4] POPPET / FREE"};
	for (int i = 0; i < 4; ++i) {
		const BaseVector base = static_cast<BaseVector>(i);
		const bool selected = player.getSpell().getBase() == base;
		const int x = static_cast<int>(panel.x + 18 + i * (panel.width - 36) / 4);
		DrawText(bases[i], x, static_cast<int>(panel.y + 48), 16, selected ? Color{245, 205, 95, 255} : line);
		if (selected) DrawLine(x, static_cast<int>(panel.y + 68), x + MeasureText(bases[i], 16), static_cast<int>(panel.y + 68), Color{245, 205, 95, 255});
	}

	const char keys[REAGENT_COUNT] = {'Z', 'X', 'C', 'V', 'B', 'N'};
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		const ReagentType type = static_cast<ReagentType>(i);
		const float cellWidth = (panel.width - 36.0f) / REAGENT_COUNT;
		const int x = static_cast<int>(panel.x + 18 + i * cellWidth);
		const int y = static_cast<int>(panel.y + 84);
		DrawRectangleLines(x, y, static_cast<int>(cellWidth - 8), 62, Fade(reagentColor(type), 0.8f));
		DrawText(TextFormat("[%c] %s", keys[i], reagentName(type)), x + 6, y + 7, 14, reagentColor(type));
		DrawText(TextFormat("x%d  %s", player.reagentCount(type), reagentShortEffect(type)), x + 6, y + 33, 13, line);
	}

	std::string chosen = "SLOTS: ";
	if (player.getSpell().getReagentCount() == 0) chosen += "EMPTY (base effect only)";
	for (int i = 0; i < player.getSpell().getReagentCount(); ++i) {
		if (i > 0) chosen += " + ";
		chosen += reagentName(player.getSpell().getReagents()[i]);
	}
	DrawText(chosen.c_str(), static_cast<int>(panel.x + 18), static_cast<int>(panel.y + 160), 16, line);
	DrawText(player.getSpell().preview().c_str(), static_cast<int>(panel.x + 18), static_cast<int>(panel.y + 188), 18, Color{245, 205, 95, 255});
	DrawText("REPEAT A KEY TO STRENGTHEN // BACKSPACE CLEARS // RELEASE Q, LEFT CLICK CASTS",
		static_cast<int>(panel.x + 18), static_cast<int>(panel.y + 217), 14, Fade(line, 0.8f));
}
