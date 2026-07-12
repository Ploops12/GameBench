#include "CoatMenu.h"

#include "Player.hxx"

#include <raylib.h>

#include <algorithm>

namespace {
	constexpr Color PANEL{10, 13, 22, 232};
	constexpr Color PANEL_LIGHT{26, 31, 43, 245};
	constexpr Color LINE{224, 230, 214, 255};
	constexpr Color ACCENT{224, 154, 90, 255};
	constexpr Color WARNING{232, 88, 88, 255};

	Rectangle baseCard(int index, int width) {
		const float cardWidth = 145.0f;
		const float gap = 12.0f;
		const float total = cardWidth * 4.0f + gap * 3.0f;
		const float start = (static_cast<float>(width) - total) * 0.5f;
		return {start + static_cast<float>(index) * (cardWidth + gap), 100.0f, cardWidth, 86.0f};
	}

	Rectangle reagentCard(int index, int width, int height) {
		const float gap = 9.0f;
		const float cardWidth = (static_cast<float>(width) - 100.0f - gap * 5.0f) / 6.0f;
		return {50.0f + static_cast<float>(index) * (cardWidth + gap), static_cast<float>(height) * 0.56f, cardWidth, 108.0f};
	}

	Rectangle slotCard(int index, int width) {
		const float cardWidth = 164.0f;
		const float gap = 12.0f;
		const float total = cardWidth * 3.0f + gap * 2.0f;
		const float start = (static_cast<float>(width) - total) * 0.5f;
		return {start + static_cast<float>(index) * (cardWidth + gap), 228.0f, cardWidth, 90.0f};
	}

	bool inside(Vector2 point, Rectangle rectangle) {
		return CheckCollisionPointRec(point, rectangle);
	}
}

CoatMenu::CoatMenu() : currentSpell(BaseVector::Poppet) {
}

void CoatMenu::update(float) {
}

void CoatMenu::update(const InputHandler::InputState& input, const Player& player) {
	if (!visible) return;
	if (input.previousBasePressed) currentSpell.cycleBase(-1);
	if (input.nextBasePressed) currentSpell.cycleBase(1);
	if (input.clearSelectionPressed) clearSelection();
	for (int i = 0; i < static_cast<int>(input.numberPressed.size()); ++i) {
		if (input.numberPressed[i]) selectReagent(ReagentFromIndex(i), player);
	}
	if (input.mouseLeftPressed) selectAtMouse(input.mousePosition, player, GetScreenWidth(), GetScreenHeight());
}

void CoatMenu::draw() const {
}

void CoatMenu::draw(const Player& player, int screenWidth, int screenHeight) const {
	if (!visible) return;
	DrawRectangle(0, 0, screenWidth, screenHeight, {3, 4, 9, 150});
	const Rectangle panel{28.0f, 28.0f, static_cast<float>(screenWidth - 56), static_cast<float>(screenHeight - 56)};
	DrawRectangleRec(panel, PANEL);
	DrawRectangleLinesEx(panel, 2.0f, LINE);
	DrawRectangleLinesEx({panel.x + 8.0f, panel.y + 8.0f, panel.width - 16.0f, panel.height - 16.0f}, 1.0f, ColorAlpha(ACCENT, 0.75f));

	DrawText("COAT RUMMAGE", 52, 48, 26, LINE);
	DrawText("WORLD LIVE", screenWidth - 198, 51, 18, Color{114, 214, 128, 255});
	DrawText("movement 38%   sprint OFF   vulnerability HIGH", 52, 76, 16, WARNING);

	DrawText("BASE VECTOR", 52, 96, 15, ColorAlpha(LINE, 0.7f));
	const BaseVector vectors[4] = {BaseVector::Bottle, BaseVector::Apple, BaseVector::Spider, BaseVector::Poppet};
	for (int i = 0; i < 4; ++i) {
		const Rectangle card = baseCard(i, screenWidth);
		const bool selected = currentSpell.base() == vectors[i];
		DrawRectangleRec(card, selected ? ColorAlpha(ACCENT, 0.3f) : PANEL_LIGHT);
		DrawRectangleLinesEx(card, selected ? 3.0f : 1.0f, selected ? ACCENT : ColorAlpha(LINE, 0.55f));
		DrawText(BaseVectorName(vectors[i]), static_cast<int>(card.x + 12.0f), static_cast<int>(card.y + 12.0f), 19, selected ? ACCENT : LINE);
		DrawText(BaseVectorHint(vectors[i]), static_cast<int>(card.x + 12.0f), static_cast<int>(card.y + 43.0f), 13, ColorAlpha(LINE, 0.75f));
	}

	DrawText("SELECTED REAGENTS  /  up to three; duplicates intensify", 52, 215, 15, ColorAlpha(LINE, 0.7f));
	for (int i = 0; i < 3; ++i) {
		const Rectangle card = slotCard(i, screenWidth);
		const bool selected = i < currentSpell.reagentCount();
		const bool cursor = i == activeSlot;
		DrawRectangleRec(card, selected ? ColorAlpha(GetReagentDefinition(currentSpell.reagentAt(i)).color, 0.17f) : PANEL_LIGHT);
		DrawRectangleLinesEx(card, cursor ? 3.0f : 1.0f, cursor ? ACCENT : ColorAlpha(LINE, 0.5f));
		DrawText(TextFormat("SLOT %d", i + 1), static_cast<int>(card.x + 10.0f), static_cast<int>(card.y + 8.0f), 13, ColorAlpha(LINE, 0.65f));
		if (selected) {
			const ReagentType type = currentSpell.reagentAt(i);
			DrawReagentIcon(type, {card.x + 43.0f, card.y + 52.0f}, 48.0f);
			DrawText(GetReagentDefinition(type).name, static_cast<int>(card.x + 76.0f), static_cast<int>(card.y + 40.0f), 14, LINE);
			DrawText(GetReagentDefinition(type).effect, static_cast<int>(card.x + 76.0f), static_cast<int>(card.y + 61.0f), 12, ColorAlpha(LINE, 0.7f));
		} else {
			DrawText("EMPTY", static_cast<int>(card.x + 55.0f), static_cast<int>(card.y + 45.0f), 16, ColorAlpha(LINE, 0.45f));
		}
	}

	DrawText("SIX SIGNS IN THE LINING", 52, static_cast<int>(screenHeight * 0.53f), 15, ColorAlpha(LINE, 0.7f));
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		const Rectangle card = reagentCard(i, screenWidth, screenHeight);
		const ReagentType type = ReagentFromIndex(i);
		const ReagentDefinition& definition = GetReagentDefinition(type);
		const int quantity = player.reagentQuantity(type);
		const int selectedCount = currentSpell.countOf(type);
		DrawRectangleRec(card, quantity > selectedCount ? ColorAlpha(definition.color, 0.11f) : ColorAlpha(WARNING, 0.1f));
		DrawRectangleLinesEx(card, 1.0f, quantity > 0 ? ColorAlpha(definition.color, 0.9f) : ColorAlpha(LINE, 0.3f));
		DrawReagentIcon(type, {card.x + card.width * 0.5f, card.y + 39.0f}, 42.0f);
		DrawText(TextFormat("%d", i + 1), static_cast<int>(card.x + 9.0f), static_cast<int>(card.y + 9.0f), 14, ACCENT);
		DrawText(definition.shortName, static_cast<int>(card.x + 8.0f), static_cast<int>(card.y + 66.0f), 13, LINE);
		DrawText(TextFormat("%d carried  /  %d used", quantity, selectedCount), static_cast<int>(card.x + 8.0f), static_cast<int>(card.y + 87.0f), 11, quantity > selectedCount ? ColorAlpha(LINE, 0.75f) : WARNING);
	}

	const Rectangle preview{50.0f, static_cast<float>(screenHeight) - 160.0f, static_cast<float>(screenWidth - 100), 75.0f};
	DrawRectangleRec(preview, ColorAlpha(PANEL_LIGHT, 0.92f));
	DrawRectangleLinesEx(preview, 2.0f, ACCENT);
	DrawText("SPELL PREVIEW", static_cast<int>(preview.x + 16.0f), static_cast<int>(preview.y + 10.0f), 13, ACCENT);
	DrawText(currentSpell.name().c_str(), static_cast<int>(preview.x + 16.0f), static_cast<int>(preview.y + 31.0f), 19, LINE);
	DrawText(currentSpell.deliveryText().c_str(), static_cast<int>(preview.x + preview.width * 0.38f), static_cast<int>(preview.y + 16.0f), 14, ColorAlpha(LINE, 0.8f));
	DrawText(currentSpell.outcomeText().c_str(), static_cast<int>(preview.x + preview.width * 0.38f), static_cast<int>(preview.y + 40.0f), 14, ColorAlpha(LINE, 0.9f));

	DrawText("1-6 choose reagent   Q/E change vector   BACKSPACE clear   release R / TAB close", 52, screenHeight - 48, 15, ColorAlpha(LINE, 0.8f));
}

void CoatMenu::open() {
	visible = true;
}

void CoatMenu::close() {
	visible = false;
}

bool CoatMenu::isOpen() const {
	return visible;
}

bool CoatMenu::selectReagent(ReagentType type, const Player& player) {
	if (activeSlot >= 3) activeSlot = 0;
	const int currentInSlot = activeSlot < currentSpell.reagentCount() && currentSpell.reagentAt(activeSlot) == type ? 1 : 0;
	if (player.reagentQuantity(type) <= currentSpell.countOf(type) - currentInSlot) return false;
	currentSpell.setReagent(activeSlot, type);
	activeSlot = (activeSlot + 1) % 3;
	return true;
}

void CoatMenu::clearSelection() {
	currentSpell.clearReagents();
	activeSlot = 0;
}

void CoatMenu::selectBase(BaseVector base) {
	currentSpell.setBase(base);
}

void CoatMenu::cycleBase(int direction) {
	currentSpell.cycleBase(direction);
}

const CurseVector& CoatMenu::spell() const {
	return currentSpell;
}

void CoatMenu::selectAtMouse(Vector2 mouse, const Player& player, int screenWidth, int screenHeight) {
	const BaseVector vectors[4] = {BaseVector::Bottle, BaseVector::Apple, BaseVector::Spider, BaseVector::Poppet};
	for (int i = 0; i < 4; ++i) {
		if (inside(mouse, baseCard(i, screenWidth))) {
			selectBase(vectors[i]);
			return;
		}
	}
	for (int i = 0; i < 3; ++i) {
		if (inside(mouse, slotCard(i, screenWidth))) {
			currentSpell.removeReagent(i);
			activeSlot = std::min(i, currentSpell.reagentCount());
			return;
		}
	}
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		if (inside(mouse, reagentCard(i, screenWidth, screenHeight))) {
			selectReagent(ReagentFromIndex(i), player);
			return;
		}
	}
}
