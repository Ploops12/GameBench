#include "CoatMenu.h"

#include <raylib.h>
#include <cstdio>
#include <cstring>

void CoatMenu::update(std::array<int, static_cast<int>(ReagentType::Count)>& inventory) {
	open = IsKeyDown(KEY_TAB) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
	if (!open) return;
	if (IsKeyPressed(KEY_ONE)) selected.base = CurseBase::Poppet;
	if (IsKeyPressed(KEY_TWO)) selected.base = CurseBase::Bottle;
	if (IsKeyPressed(KEY_THREE)) selected.base = CurseBase::Apple;
	if (IsKeyPressed(KEY_FOUR)) selected.base = CurseBase::Spider;
	const int reagentKeys[] = {KEY_Q, KEY_W, KEY_R, KEY_T, KEY_Y, KEY_U};
	for (int index = 0; index < static_cast<int>(ReagentType::Count); ++index) {
		int alreadySelected = 0;
		for (int slot = 0; slot < selected.reagentCount; ++slot) if (selected.reagents[slot] == static_cast<ReagentType>(index)) ++alreadySelected;
		if (IsKeyPressed(reagentKeys[index]) && inventory[index] > alreadySelected && selected.reagentCount < 3) {
			selected.reagents[selected.reagentCount++] = static_cast<ReagentType>(index);
		}
	}
	if (IsKeyPressed(KEY_BACKSPACE)) selected.reagentCount = 0;
	selected.rebuild();
}

void CoatMenu::clearSpell() { selected.reagentCount = 0; selected.rebuild(); }

void CoatMenu::draw(int w, int h, const std::array<int, static_cast<int>(ReagentType::Count)>& inventory, float penalty) const {
	int panelW = w - 48;
	DrawRectangle(24, h - 292, panelW, 260, Fade({18, 23, 25, 255}, 0.93f));
	DrawRectangleLines(24, h - 292, panelW, 260, {203, 214, 188, 255});
	DrawText("COAT RUMMAGE  //  TIME CONTINUES", 44, h - 276, 20, {238, 242, 220, 255});
	DrawText("VULNERABLE: SLOW 45%  -  SPRINT & JUMP SEALED", 44, h - 248, 16, {237, 168, 89, 255});
	const char* bases[] = {"1 POPPET", "2 BOTTLE", "3 APPLE", "4 SPIDER"};
	for (int i = 0; i < 4; ++i) {
		Color c = static_cast<int>(selected.base) == i ? Color{113, 224, 166, 255} : Color{155, 165, 153, 255};
		DrawText(bases[i], 44 + i * 142, h - 215, 17, c);
	}
	for (int i = 0; i < static_cast<int>(ReagentType::Count); ++i) {
		int x = 44 + (i % 3) * 190;
		int y = h - 177 + (i / 3) * 31;
		Color c = inventory[i] > 0 ? Color{224, 212, 157, 255} : Color{95, 99, 92, 255};
		const char* labels[] = {"Q", "W", "R", "T", "Y", "U"};
		DrawText(TextFormat("%s  %-14s x%d", labels[i], reagentName(static_cast<ReagentType>(i)), inventory[i]), x, y, 16, c);
	}
	char reagents[100] = "none";
	if (selected.reagentCount) {
		reagents[0] = '\0';
		for (int i = 0; i < selected.reagentCount; ++i) {
			if (i) std::snprintf(reagents + std::strlen(reagents), sizeof(reagents) - std::strlen(reagents), ", ");
			std::snprintf(reagents + std::strlen(reagents), sizeof(reagents) - std::strlen(reagents), "%s", reagentName(selected.reagents[i]));
		}
	}
	DrawText(TextFormat("PREVIEW: %s / %s", selected.baseName(), selected.deliveryName()), 630, h - 215, 17, {113, 224, 166, 255});
	DrawText(TextFormat("[%s]", reagents), 630, h - 187, 15, {224, 212, 157, 255});
	DrawText(TextFormat("DMG %.0f  RAD %.1f  %s%s%s%s", selected.damage, selected.radius, selected.poison ? "ROT " : "", selected.snare ? "SNARE " : "", selected.fear ? "FEAR " : "", selected.seeking ? "SEEK" : ""), 630, h - 157, 15, {238, 242, 220, 255});
	DrawText("BACKSPACE: CLEAR     RELEASE TAB / RMB, THEN LMB: CAST", 630, h - 116, 14, {155, 165, 153, 255});
	(void)penalty;
}
