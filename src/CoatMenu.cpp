#include "CoatMenu.h"

#include <algorithm>
#include <array>

#include <raylib.h>

namespace {

struct CoatLayout {
	Rectangle panel {};
	std::array<Rectangle, 4> bases {};
	std::array<Rectangle, ReagentCount> reagents {};
	Rectangle castButton {};
};

CoatLayout makeLayout(int screenWidth, int screenHeight) {
	CoatLayout layout {};
	const float panelWidth = std::min(640.0f, static_cast<float>(screenWidth) - 48.0f);
	const float panelHeight = std::min(640.0f, static_cast<float>(screenHeight) - 48.0f);
	layout.panel = {static_cast<float>(screenWidth) - panelWidth - 24.0f, 24.0f, panelWidth, panelHeight};

	const float baseGap = 8.0f;
	const float baseWidth = (panelWidth - 44.0f - baseGap * 3.0f) / 4.0f;
	for (std::size_t index = 0; index < layout.bases.size(); ++index) {
		layout.bases[index] = {
			layout.panel.x + 22.0f + static_cast<float>(index) * (baseWidth + baseGap),
			layout.panel.y + 58.0f,
			baseWidth,
			58.0f
		};
	}

	const float reagentGap = 8.0f;
	const float reagentWidth = (panelWidth - 44.0f - reagentGap * 2.0f) / 3.0f;
	for (std::size_t index = 0; index < ReagentCount; ++index) {
		const int row = static_cast<int>(index / 3U);
		const int column = static_cast<int>(index % 3U);
		layout.reagents[index] = {
			layout.panel.x + 22.0f + static_cast<float>(column) * (reagentWidth + reagentGap),
			layout.panel.y + 190.0f + static_cast<float>(row) * 88.0f,
			reagentWidth,
			76.0f
		};
	}

	layout.castButton = {layout.panel.x + panelWidth - 198.0f, layout.panel.y + panelHeight - 52.0f, 176.0f, 30.0f};
	return layout;
}

void drawButton(Rectangle rectangle, const char* label, const char* detail, bool selected, Color color) {
	const Color border = selected ? color : Fade({162, 184, 203, 255}, 0.72f);
	const Color fill = selected ? Fade(color, 0.20f) : Fade({17, 24, 38, 255}, 0.82f);
	DrawRectangleRec(rectangle, fill);
	DrawRectangleLinesEx(rectangle, selected ? 2.5f : 1.0f, border);
	DrawText(label, static_cast<int>(rectangle.x + 8.0f), static_cast<int>(rectangle.y + 7.0f), 16, selected ? RAYWHITE : border);
	DrawText(detail, static_cast<int>(rectangle.x + 8.0f), static_cast<int>(rectangle.y + 30.0f), 12, Fade(border, 0.86f));
}

bool hasSelection(const CurseVector& spell, ReagentType type) {
	for (int index = 0; index < spell.getReagentCount(); ++index) {
		if (spell.getReagents()[static_cast<std::size_t>(index)] == type) {
			return true;
		}
	}
	return false;
}

}

CoatMenu::CoatMenu()
	: spell(CurseVectorType::Poppet) {
}

CoatMenuInputResult CoatMenu::update(const InputHandler::InputState& input,
	const std::array<int, ReagentCount>& inventory, int screenWidth, int screenHeight) {
	CoatMenuInputResult result {};
	displayedInventory = inventory;
	if (input.rummageTogglePressed) {
		toggledOpen = !toggledOpen;
	}
	open = input.rummageHeld || toggledOpen;
	if (!open) {
		return result;
	}

	if (input.quickKey == 1) chooseBase(CurseVectorType::Bottle);
	if (input.quickKey == 2) chooseBase(CurseVectorType::Apple);
	if (input.quickKey == 3) chooseBase(CurseVectorType::Spider);
	if (input.quickKey == 0) chooseBase(CurseVectorType::Poppet);
	if (input.quickKey >= 4 && input.quickKey <= 9) {
		const ReagentType type = reagentFromIndex(static_cast<std::size_t>(input.quickKey - 4));
		chooseReagent(type, inventory[reagentIndex(type)]);
	}
	if (input.removeReagentPressed) {
		statusText = spell.removeLastReagent() ? "Last reagent returned to the coat." : "No reagent is selected.";
	}
	if (input.clearReagentsPressed) {
		spell.clearReagents();
		statusText = "Pockets cleared.  The base vector remains.";
	}

	const CoatLayout layout = makeLayout(screenWidth, screenHeight);
	if (input.castPressed) {
		const Vector2 mouse = GetMousePosition();
		for (std::size_t index = 0; index < layout.bases.size(); ++index) {
			if (CheckCollisionPointRec(mouse, layout.bases[index])) {
				chooseBase(static_cast<CurseVectorType>(index));
				result.consumedMouseClick = true;
				return result;
			}
		}
		for (std::size_t index = 0; index < ReagentCount; ++index) {
			if (CheckCollisionPointRec(mouse, layout.reagents[index])) {
				const ReagentType type = reagentFromIndex(index);
				chooseReagent(type, inventory[index]);
				result.consumedMouseClick = true;
				return result;
			}
		}
		if (CheckCollisionPointRec(mouse, layout.castButton)) {
			result.consumedMouseClick = true;
			result.castRequested = true;
		}
	}

	return result;
}

void CoatMenu::draw(int screenWidth, int screenHeight) const {
	if (!open) {
		return;
	}

	const CoatLayout layout = makeLayout(screenWidth, screenHeight);
	const Color panelInk {9, 15, 26, 239};
	const Color seam {137, 232, 255, 255};
	const Color warning {255, 143, 106, 255};
	const Color spellColor = spell.getStats().color;

	DrawRectangleRec(layout.panel, panelInk);
	DrawRectangleLinesEx(layout.panel, 2.0f, seam);
	DrawLineEx({layout.panel.x + 12.0f, layout.panel.y + 40.0f}, {layout.panel.x + layout.panel.width - 12.0f, layout.panel.y + 40.0f}, 1.0f, Fade(seam, 0.65f));
	DrawText("COAT RUMMAGE", static_cast<int>(layout.panel.x + 20.0f), static_cast<int>(layout.panel.y + 12.0f), 24, RAYWHITE);
	DrawText("LIVE WORLD  |  VULNERABLE: 46% MOVE, NO SPRINT / JUMP", static_cast<int>(layout.panel.x + 222.0f),
		static_cast<int>(layout.panel.y + 17.0f), 12, warning);

	const char* baseLabels[] {"[1] BOTTLE", "[2] APPLE", "[3] SPIDER", "[0] POPPET"};
	const char* baseDetails[] {"thrown", "self pulse", "placed mine", "free fallback"};
	for (std::size_t index = 0; index < layout.bases.size(); ++index) {
		drawButton(layout.bases[index], baseLabels[index], baseDetails[index],
			spell.getBase() == static_cast<CurseVectorType>(index), spellColor);
	}

	DrawText("THREE STITCHED SLOTS  (same reagent = stronger)", static_cast<int>(layout.panel.x + 22.0f),
		static_cast<int>(layout.panel.y + 132.0f), 13, seam);
	for (int index = 0; index < CurseVector::MaxReagents; ++index) {
		const Rectangle slot {layout.panel.x + 22.0f + static_cast<float>(index) * 178.0f, layout.panel.y + 150.0f, 162.0f, 26.0f};
		DrawRectangleRec(slot, Fade({24, 32, 49, 255}, 0.85f));
		DrawRectangleLinesEx(slot, 1.0f, Fade(seam, 0.7f));
		if (index < spell.getReagentCount()) {
			const ReagentType type = spell.getReagents()[static_cast<std::size_t>(index)];
			DrawText(getReagentDefinition(type).name, static_cast<int>(slot.x + 8.0f), static_cast<int>(slot.y + 5.0f), 15,
				getReagentDefinition(type).color);
		} else {
			DrawText("EMPTY", static_cast<int>(slot.x + 8.0f), static_cast<int>(slot.y + 5.0f), 14, Fade(LIGHTGRAY, 0.55f));
		}
	}

	for (std::size_t index = 0; index < ReagentCount; ++index) {
		const ReagentType type = reagentFromIndex(index);
		const ReagentDefinition& definition = getReagentDefinition(type);
		const bool selected = hasSelection(spell, type);
		const bool unavailable = displayedInventory[index] == 0;
		const Rectangle card = layout.reagents[index];
		drawButton(card, ("[" + std::to_string(index + 4U) + "] " + definition.name).c_str(), definition.effect,
			selected, unavailable ? Fade(definition.color, 0.36f) : definition.color);
		DrawText(("x" + std::to_string(displayedInventory[index])).c_str(), static_cast<int>(card.x + card.width - 33.0f),
			static_cast<int>(card.y + 7.0f), 16, unavailable ? GRAY : RAYWHITE);
	}

	const Rectangle preview {layout.panel.x + 22.0f, layout.panel.y + 382.0f, layout.panel.width - 44.0f, 128.0f};
	DrawRectangleRec(preview, Fade({19, 28, 43, 255}, 0.92f));
	DrawRectangleLinesEx(preview, 1.5f, spellColor);
	DrawText("RESULT: ", static_cast<int>(preview.x + 10.0f), static_cast<int>(preview.y + 9.0f), 16, seam);
	DrawText(spell.getDisplayName().c_str(), static_cast<int>(preview.x + 80.0f), static_cast<int>(preview.y + 9.0f), 16, spellColor);
	const std::vector<std::string> previewLines = spell.getPreviewLines();
	for (std::size_t index = 0; index < previewLines.size() && index < 6U; ++index) {
		DrawText(previewLines[index].c_str(), static_cast<int>(preview.x + 10.0f), static_cast<int>(preview.y + 33.0f + static_cast<float>(index) * 15.0f),
			12, index == 0 ? RAYWHITE : Fade(LIGHTGRAY, 0.9f));
	}

	DrawText(statusText.c_str(), static_cast<int>(layout.panel.x + 22.0f), static_cast<int>(layout.panel.y + layout.panel.height - 42.0f), 13,
		Fade(LIGHTGRAY, 0.9f));
	DrawRectangleRec(layout.castButton, Fade(spellColor, 0.18f));
	DrawRectangleLinesEx(layout.castButton, 2.0f, spellColor);
	DrawText("CAST [LMB]", static_cast<int>(layout.castButton.x + 20.0f), static_cast<int>(layout.castButton.y + 7.0f), 16, RAYWHITE);
	DrawText("BACKSPACE remove  |  C clear  |  release R closes", static_cast<int>(layout.panel.x + 22.0f),
		static_cast<int>(layout.panel.y + layout.panel.height - 20.0f), 12, Fade(seam, 0.75f));
}

const CurseVector& CoatMenu::getSpell() const {
	return spell;
}

CurseVector& CoatMenu::getSpell() {
	return spell;
}

const std::string& CoatMenu::getStatusText() const {
	return statusText;
}

void CoatMenu::chooseBase(CurseVectorType type) {
	spell.setBase(type);
	statusText = std::string(CurseVector::getBaseName(type)) + " stitched into your hand.";
}

void CoatMenu::chooseReagent(ReagentType type, int availableAmount) {
	if (spell.tryAddReagent(type, availableAmount)) {
		statusText = std::string(getReagentDefinition(type).name) + " added.  Duplicates deepen the same curse.";
	} else if (availableAmount <= 0) {
		statusText = std::string("No ") + getReagentDefinition(type).name + " in the coat.";
	} else {
		statusText = "All three stitched slots are full, or that stack is spent.";
	}
}
