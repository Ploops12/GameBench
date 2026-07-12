#include "CoatMenu.h"

CoatMenu::CoatMenu() {
	visible = false;
}

void CoatMenu::update(const InputHandler::InputState& input, const ReagentInventory& inventory) {
	visible = input.rummageHeld;

	for (int i = 0; i < 4; ++i) {
		if (input.basePressed[i]) {
			recipe.base = static_cast<BaseVectorType>(i);
		}
	}

	if (!visible) {
		return;
	}

	for (const ReagentInfo& info : getReagentInfos()) {
		if (input.reagentPressed[getReagentIndex(info.type)]) {
			recipe.addReagent(info.type, inventory[getReagentIndex(info.type)]);
		}
	}

	if (input.removePressed) {
		recipe.popSelection();
	}

	if (input.clearPressed) {
		recipe.clearReagents();
	}
}

void CoatMenu::draw(const ReagentInventory& inventory) const {
	if (!visible) {
		return;
	}

	const int screenW = GetScreenWidth();
	const int screenH = GetScreenHeight();
	const Rectangle panel {
		20.0f,
		static_cast<float>(screenH) - 250.0f,
		static_cast<float>(screenW) - 40.0f,
		230.0f
	};

	DrawRectangleRounded(panel, 0.04f, 8, Fade(BLACK, 0.78f));
	DrawRectangleRoundedLinesEx(panel, 0.04f, 8, 2.0f, LIGHTGRAY);
	DrawText("COAT RUMMAGE  Hold TAB", static_cast<int>(panel.x + 18.0f), static_cast<int>(panel.y + 14.0f), 22, RAYWHITE);
	DrawText("1 Poppet  2 Bottle  3 Apple  4 Spider", static_cast<int>(panel.x + 18.0f), static_cast<int>(panel.y + 42.0f), 18, GRAY);
	DrawText("Backspace remove  C clear  Sprint disabled  Vulnerable +25%", static_cast<int>(panel.x + 18.0f), static_cast<int>(panel.y + 66.0f), 18, {255, 170, 170, 255});

	const int previewX = static_cast<int>(panel.x + panel.width - 360.0f);
	DrawText("Selected Ingredients", previewX, static_cast<int>(panel.y + 84.0f), 18, LIGHTGRAY);
	for (int slot = 0; slot < 3; ++slot) {
		Rectangle slotBox {static_cast<float>(previewX + slot * 108), static_cast<float>(panel.y + 108.0f), 96.0f, 36.0f};
		bool filled = slot < recipe.slotCount;
		Color fillColor = Fade(filled ? getReagentInfo(recipe.slots[slot]).color : DARKGRAY, filled ? 0.65f : 0.35f);
		DrawRectangleRounded(slotBox, 0.15f, 6, fillColor);
		DrawRectangleRoundedLinesEx(slotBox, 0.15f, 6, 2.0f, filled ? WHITE : GRAY);
		DrawText(filled ? getReagentInfo(recipe.slots[slot]).shortName : "-", static_cast<int>(slotBox.x + 10.0f), static_cast<int>(slotBox.y + 8.0f), 18, WHITE);
	}

	int x = static_cast<int>(panel.x + 18.0f);
	int y = static_cast<int>(panel.y + 102.0f);
	for (int i = 0; i < 4; ++i) {
		BaseVectorType base = static_cast<BaseVectorType>(i);
		const bool selected = recipe.base == base;
		Rectangle box {static_cast<float>(x + i * 120), static_cast<float>(y), 104.0f, 40.0f};
		DrawRectangleRounded(box, 0.15f, 6, selected ? Fade(getBaseVectorColor(base), 0.8f) : Fade(DARKGRAY, 0.6f));
		DrawRectangleRoundedLinesEx(box, 0.15f, 6, 2.0f, selected ? WHITE : GRAY);
		DrawText(getBaseVectorName(base), static_cast<int>(box.x + 10.0f), static_cast<int>(box.y + 9.0f), 20, WHITE);
	}

	y += 58;
	for (int i = 0; i < static_cast<int>(getReagentInfos().size()); ++i) {
		const ReagentInfo& info = getReagentInfos()[i];
		const bool active = recipe.getCount(info.type) > 0;
		Rectangle box {static_cast<float>(x + (i % 3) * 200), static_cast<float>(y + (i / 3) * 54), 184.0f, 42.0f};
		DrawRectangleRounded(box, 0.15f, 6, active ? Fade(info.color, 0.55f) : Fade(BLACK, 0.45f));
		DrawRectangleRoundedLinesEx(box, 0.15f, 6, 2.0f, active ? info.color : GRAY);
		DrawText(TextFormat("%c  %s", static_cast<char>(info.keybind), info.name), static_cast<int>(box.x + 10.0f), static_cast<int>(box.y + 7.0f), 18, WHITE);
		DrawText(info.effectLabel, static_cast<int>(box.x + 10.0f), static_cast<int>(box.y + 24.0f), 14, Fade(WHITE, 0.75f));
		DrawText(TextFormat("%d / %d", recipe.getCount(info.type), inventory[i]), static_cast<int>(box.x + 128.0f), static_cast<int>(box.y + 9.0f), 18, active ? WHITE : LIGHTGRAY);
	}

	ComposedSpell preview = getPreview();
	DrawText(preview.label.c_str(), previewX, static_cast<int>(panel.y + 18.0f), 24, getBaseVectorColor(preview.base));
	DrawText(preview.preview.c_str(), previewX, static_cast<int>(panel.y + 52.0f), 18, RAYWHITE);
}

const SpellRecipe& CoatMenu::getRecipe() const {
	return recipe;
}

ComposedSpell CoatMenu::getPreview() const {
	return composeSpell(recipe);
}

float CoatMenu::getMovementScale() const {
	return visible ? 0.45f : 1.0f;
}
