#include "CoatMenu.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

#include <raylib.h>

namespace {
int reagentCount(const std::vector<ReagentType>& selected, ReagentType reagent) {
	return static_cast<int>(std::count(selected.begin(), selected.end(), reagent));
}

void panel(Rectangle rectangle, Color border) {
	DrawRectangleRec(rectangle, Fade(BLACK, 0.78f));
	DrawRectangleLinesEx(rectangle, 2.0f, border);
}
}

void CoatMenu::setOpen(bool value) { open = value; }
bool CoatMenu::isOpen() const { return open; }

void CoatMenu::update(const std::array<int, REAGENT_COUNT>& inventory) {
	if (!open) return;
	if (IsKeyPressed(KEY_B)) base = static_cast<BaseVector>((static_cast<int>(base) + 1) % 4);
	for (int index = 0; index < REAGENT_COUNT; ++index) {
		if (IsKeyPressed(KEY_ONE + index)) highlighted = index;
	}
	ReagentType reagent = static_cast<ReagentType>(highlighted);
	if (IsKeyPressed(KEY_SPACE) && canAdd(reagent, inventory)) selected.push_back(reagent);
	if (IsKeyPressed(KEY_BACKSPACE) && !selected.empty()) selected.pop_back();
	if (IsKeyPressed(KEY_X)) selected.clear();
}

void CoatMenu::draw(const std::array<int, REAGENT_COUNT>& inventory) const {
	if (!open) return;
	int width = GetScreenWidth();
	int height = GetScreenHeight();
	Rectangle mainPanel {18.0f, static_cast<float>(height - 230), static_cast<float>(width - 36), 212.0f};
	panel(mainPanel, {174, 132, 239, 255});
	DrawText("COAT RUMMAGE  //  VULNERABLE: MOVEMENT SLOWED - NO SPRINT OR JUMP", 34, height - 216, 20, {233, 222, 189, 255});
	DrawText("[B] VECTOR    [1-6] POCKET    [SPACE] ADD    [BACKSPACE] REMOVE    [X] CLEAR", 34, height - 190, 14, LIGHTGRAY);

	SpellStats preview = getPreview();
	char title[160];
	std::snprintf(title, sizeof(title), "VECTOR: %s     ASSEMBLED: %d / 3", CurseVector::getName(base), static_cast<int>(selected.size()));
	DrawText(title, 34, height - 163, 18, RAYWHITE);
	for (int index = 0; index < REAGENT_COUNT; ++index) {
		const Reagent& reagent = getReagent(static_cast<ReagentType>(index));
		float x = 34.0f + index * 112.0f;
		Rectangle slot {x, static_cast<float>(height - 135), 102.0f, 62.0f};
		Color border = index == highlighted ? RAYWHITE : reagent.color;
		panel(slot, border);
		char amount[12];
		std::snprintf(amount, sizeof(amount), "%d", inventory[index]);
		DrawText(TextFormat("%d", index + 1), static_cast<int>(x + 6), height - 128, 16, reagent.color);
		DrawText(amount, static_cast<int>(x + 80), height - 128, 16, RAYWHITE);
		DrawText(reagent.name, static_cast<int>(x + 6), height - 106, 12, RAYWHITE);
		DrawText(reagent.shortEffect, static_cast<int>(x + 6), height - 88, 10, Fade(reagent.color, 0.92f));
	}

	int previewX = 716;
	char details[240];
	std::snprintf(details, sizeof(details), "OUTCOME: DMG %.0f  RADIUS %.1f  WARD +%.0f", preview.damage, preview.radius, preview.wardGain);
	DrawText(details, previewX, height - 163, 15, {111, 216, 237, 255});
	std::snprintf(details, sizeof(details), "ROT %.0f/s %.0fs  SNARE %.1fs  PANIC %.1fs", preview.poisonDamagePerSecond, preview.poisonDuration, preview.snareDuration, preview.fearDuration);
	DrawText(details, previewX, height - 140, 14, {226, 183, 64, 255});
	std::snprintf(details, sizeof(details), "LINGER %.1fs  SEEK %.0fm", preview.lingerDuration, preview.seekRadius);
	DrawText(details, previewX, height - 118, 14, {229, 91, 108, 255});
	if (selected.empty()) {
		DrawText("SELECTED: none", previewX, height - 94, 14, LIGHTGRAY);
	} else {
		char selection[240] {"SELECTED: "};
		for (std::size_t index = 0; index < selected.size(); ++index) {
			if (index > 0) std::strncat(selection, " + ", sizeof(selection) - std::strlen(selection) - 1);
			std::strncat(selection, getReagent(selected[index]).name, sizeof(selection) - std::strlen(selection) - 1);
		}
		DrawText(selection, previewX, height - 94, 14, RAYWHITE);
	}
}

BaseVector CoatMenu::getBase() const { return base; }
const std::vector<ReagentType>& CoatMenu::getSelected() const { return selected; }
SpellStats CoatMenu::getPreview() const { return CurseVector::build(base, selected); }

bool CoatMenu::consume(std::array<int, REAGENT_COUNT>& inventory) {
	for (ReagentType reagent : selected) {
		int index = static_cast<int>(reagent);
		if (reagentCount(selected, reagent) > inventory[index]) return false;
	}
	for (ReagentType reagent : selected) --inventory[static_cast<int>(reagent)];
	selected.clear();
	return true;
}

bool CoatMenu::canAdd(ReagentType reagent, const std::array<int, REAGENT_COUNT>& inventory) const {
	return selected.size() < 3 && reagentCount(selected, reagent) < inventory[static_cast<int>(reagent)];
}
