#include "CoatMenu.h"

bool CoatMenu::update(Player &player) {
	open = IsKeyDown(KEY_TAB);
	player.setRummaging(open);
	if (!open) return false;
	if (IsKeyPressed(KEY_Q)) baseCursor = (baseCursor + 3) % 4;
	if (IsKeyPressed(KEY_E)) baseCursor = (baseCursor + 1) % 4;
	if (IsKeyPressed(KEY_BACKSPACE)) player.spell().removeLastReagent();
	if (IsKeyPressed(KEY_ONE)) cursor = 0;
	if (IsKeyPressed(KEY_TWO)) cursor = 1;
	if (IsKeyPressed(KEY_THREE)) cursor = 2;
	if (IsKeyPressed(KEY_FOUR)) cursor = 3;
	if (IsKeyPressed(KEY_FIVE)) cursor = 4;
	if (IsKeyPressed(KEY_SIX)) cursor = 5;
	if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		int selected = 0;
		for (int slot : player.spell().getReagents()) if (slot == cursor) ++selected;
		if (selected < player.reagentCount(static_cast<ReagentType>(cursor))) player.spell().addReagent(static_cast<ReagentType>(cursor));
	}
	const BaseVector base = static_cast<BaseVector>(baseCursor);
	if (base != player.spell().getBase()) { player.spell().setBase(base); player.spell().clearReagents(); }
	return true;
}

void CoatMenu::draw(const Player &player) const {
	if (!open) return;
	const int w = GetScreenWidth();
	const int h = GetScreenHeight();
	DrawRectangle(38, 42, w - 76, h - 84, Color{12, 16, 24, 238});
	DrawRectangleLinesEx(Rectangle{38, 42, static_cast<float>(w - 76), static_cast<float>(h - 84)}, 3.0f, Color{220, 190, 110, 255});
	DrawText("COAT RUMMAGE", 66, 64, 28, Color{255, 230, 160, 255});
	DrawText("WORLD LIVE  /  MOVE SLOWED  /  SPRINT DISABLED", 68, 99, 14, Color{255, 120, 120, 255});
	DrawText("BASE VECTOR  [Q/E]", 70, 145, 16, LIGHTGRAY);
	const char *bases[] = {"BOTTLE  throw", "APPLE  eat", "SPIDER  mine", "POPPET  weak"};
	for (int i = 0; i < 4; ++i) DrawText(TextFormat("%c  %s", i == baseCursor ? '>' : ' ', bases[i]), 78, 174 + i * 25, 18, i == baseCursor ? Color{255, 230, 120, 255} : LIGHTGRAY);
	DrawText("REAGENTS  [1-6] + ENTER / CLICK", w / 2, 145, 16, LIGHTGRAY);
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		const int x = w / 2 + (i % 3) * 145;
		const int y = 176 + (i / 3) * 72;
		const ReagentDefinition &info = getReagentDefinition(static_cast<ReagentType>(i));
		DrawRectangleLines(x, y, 128, 58, i == cursor ? info.color : Color{75, 85, 100, 255});
		DrawCircle(x + 18, y + 29, 9.0f, info.color);
		DrawText(TextFormat("%d  %s", i + 1, info.shortName), x + 34, y + 10, 14, RAYWHITE);
		DrawText(TextFormat("x%d  %s", player.reagentCount(static_cast<ReagentType>(i)), info.effect), x + 34, y + 32, 12, LIGHTGRAY);
	}
	int selected = 0;
	for (int slot : player.spell().getReagents()) if (slot >= 0) ++selected;
	DrawText("SPELL PREVIEW", 70, h - 190, 16, LIGHTGRAY);
	DrawText(player.spell().preview().c_str(), 70, h - 160, 18, Color{140, 240, 190, 255});
	DrawText(TextFormat("SLOTS: %d / 3", selected), 70, h - 128, 14, LIGHTGRAY);
	DrawText("SELECTED:", 70, h - 105, 13, LIGHTGRAY);
	int selectedX = 145;
	for (int slot : player.spell().getReagents()) if (slot >= 0) {
		DrawText(getReagentDefinition(static_cast<ReagentType>(slot)).shortName, selectedX, h - 105, 13, getReagentDefinition(static_cast<ReagentType>(slot)).color);
		selectedX += 72;
	}
	DrawText("TAB release  |  BACKSPACE remove last  |  R activates ward", 70, h - 82, 14, Color{240, 210, 160, 255});
}
