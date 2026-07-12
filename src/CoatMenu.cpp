#include "CoatMenu.h"

#include <raylib.h>

#include <algorithm>
#include <array>
#include <utility>

namespace {
void DrawPanel(Rectangle rectangle, Color lineColor, Color fillColor = {4, 8, 12, 222}) {
	DrawRectangleRec(rectangle, fillColor);
	DrawRectangleLinesEx(rectangle, 2.0f, lineColor);
}

const char* DeliveryText(BaseVectorType base) {
	switch (base) {
	case BaseVectorType::Bottle: return "THROWN // DETONATES ON IMPACT";
	case BaseVectorType::Apple: return "EATEN // HEX AURA FOLLOWS YOU";
	case BaseVectorType::Spider: return "PLACED // TRIGGERS ON APPROACH";
	case BaseVectorType::Poppet: return "INFINITE // WEAK LINE-OF-SIGHT JAB";
	}
	return "";
}
}

CoatMenu::CoatMenu() {
	active_ = false;
}

void CoatMenu::update(const InputHandler::InputState& input, float dt, const Player& player, CurseVector& spell) {
	drawPlayer_ = &player;
	drawSpell_ = &spell;
	statusTimer_ = std::max(0.0f, statusTimer_ - dt);
	if (statusTimer_ <= 0.0f) {
		statusMessage_.clear();
	}
	active_ = input.rummageHeld;
	if (!active_) {
		return;
	}

	if (input.baseSelection >= 0) {
		static const std::array<BaseVectorType, 4> bases {
			BaseVectorType::Bottle,
			BaseVectorType::Apple,
			BaseVectorType::Spider,
			BaseVectorType::Poppet
		};
		spell.selectBase(bases[static_cast<std::size_t>(input.baseSelection)]);
		setStatus(spell.base() == BaseVectorType::Poppet ? "POPPET READY // REAGENTS CLEARED" : spell.baseName() + " VECTOR READY");
	}

	if (input.clearPressed) {
		spell.clearReagents();
		setStatus("INGREDIENT SLOTS CLEARED");
	} else if (input.undoPressed) {
		setStatus(spell.removeLastReagent() ? "LAST INGREDIENT RETURNED" : "NO INGREDIENT TO RETURN");
	}

	for (std::size_t i = 0; i < input.reagentPressed.size(); ++i) {
		if (!input.reagentPressed[i]) {
			continue;
		}
		const ReagentType type = static_cast<ReagentType>(i);
		const auto& definition = GetReagentDefinition(type);
		if (spell.base() == BaseVectorType::Poppet) {
			setStatus("CHOOSE BOTTLE, APPLE, OR SPIDER FIRST");
			continue;
		}
		if (spell.reagents().size() >= CurseVector::MAX_REAGENTS) {
			setStatus("ALL THREE INGREDIENT LOOPS ARE FULL");
			continue;
		}
		if (spell.reagentCount(type) >= player.reagentQuantity(type)) {
			setStatus(std::string("NO MORE ") + std::string(definition.shortName) + " IN THE COAT");
			continue;
		}
		if (spell.addReagent(type)) {
			setStatus(std::string(definition.shortName) + " ADDED // STACKS STRENGTHEN IT");
		}
	}
}

void CoatMenu::draw(const Player& player, const CurseVector& spell) const {
	if (!active_) {
		return;
	}
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	const float margin = std::max(18.0f, width * 0.018f);
	const float panelY = height * 0.405f;
	const float panelHeight = height - panelY - margin;
	const Color cyan {87, 220, 224, 255};
	const Color danger {255, 78, 91, 255};
	const Color text {218, 232, 225, 255};
	const Color dim {75, 119, 126, 255};

	DrawRectangle(0, static_cast<int>(panelY - 13.0f), width, static_cast<int>(panelHeight + margin + 13.0f), {2, 5, 8, 148});
	DrawPanel({margin, panelY, width - margin * 2.0f, panelHeight}, cyan);
	DrawRectangle(static_cast<int>(width * 0.34f), static_cast<int>(panelY - 37.0f), static_cast<int>(width * 0.32f), 30, {21, 5, 9, 235});
	DrawRectangleLines(static_cast<int>(width * 0.34f), static_cast<int>(panelY - 37.0f), static_cast<int>(width * 0.32f), 30, danger);
	const char* warning = "COAT OPEN // WORLD LIVE // SPRINT & JUMP LOCKED // +25% HURT";
	DrawText(warning, width / 2 - MeasureText(warning, 14) / 2, static_cast<int>(panelY - 29.0f), 14, danger);

	const float leftWidth = (width - margin * 3.0f) * 0.64f;
	const Rectangle left {margin * 1.5f, panelY + 18.0f, leftWidth, panelHeight - 36.0f};
	const Rectangle preview {left.x + left.width + margin, left.y, width - left.x - left.width - margin * 2.5f, left.height};
	DrawText("BASE VECTOR  [1-4]", static_cast<int>(left.x), static_cast<int>(left.y), 17, cyan);

	static const std::array<const char*, 4> baseNames {"1 BOTTLE", "2 APPLE", "3 SPIDER", "4 POPPET"};
	static const std::array<BaseVectorType, 4> bases {BaseVectorType::Bottle, BaseVectorType::Apple, BaseVectorType::Spider, BaseVectorType::Poppet};
	const float baseGap = 7.0f;
	const float baseWidth = (left.width - baseGap * 3.0f) / 4.0f;
	for (std::size_t i = 0; i < bases.size(); ++i) {
		const Rectangle card {left.x + static_cast<float>(i) * (baseWidth + baseGap), left.y + 26.0f, baseWidth, 43.0f};
		const bool selected = spell.base() == bases[i];
		DrawPanel(card, selected ? danger : dim, selected ? Color {25, 8, 13, 235} : Color {4, 8, 12, 215});
		const int fontSize = width < 900 ? 13 : 15;
		DrawText(baseNames[i], static_cast<int>(card.x + card.width / 2.0f - MeasureText(baseNames[i], fontSize) / 2),
			static_cast<int>(card.y + 13.0f), fontSize, selected ? danger : text);
	}

	DrawText("INGREDIENT LOOPS  [Z X C / V B N]", static_cast<int>(left.x), static_cast<int>(left.y + 82.0f), 17, cyan);
	const auto& definitions = GetReagentDefinitions();
	const float reagentGap = 7.0f;
	const float reagentWidth = (left.width - reagentGap * 2.0f) / 3.0f;
	const float reagentHeight = std::max(53.0f, (left.height - 145.0f) / 2.0f);
	for (std::size_t i = 0; i < definitions.size(); ++i) {
		const int row = static_cast<int>(i / 3);
		const int column = static_cast<int>(i % 3);
		const Rectangle card {
			left.x + column * (reagentWidth + reagentGap),
			left.y + 108.0f + row * (reagentHeight + reagentGap),
			reagentWidth,
			reagentHeight
		};
		const auto& definition = definitions[i];
		const int available = player.reagentQuantity(definition.type);
		const int selected = spell.reagentCount(definition.type);
		const Color cardColor = available > 0 ? definition.color : dim;
		DrawPanel(card, selected > 0 ? definition.color : ColorAlpha(cardColor, 0.58f), {4, 8, 12, 215});
		DrawText(TextFormat("[%s] %s", definition.keyLabel.data(), definition.name.data()), static_cast<int>(card.x + 8.0f),
			static_cast<int>(card.y + 7.0f), width < 900 ? 12 : 14, cardColor);
		DrawText(TextFormat("COAT %02i   LOOP %i", available, selected), static_cast<int>(card.x + 8.0f),
			static_cast<int>(card.y + 27.0f), 13, selected > 0 ? text : dim);
		DrawText(definition.effect.data(), static_cast<int>(card.x + 8.0f), static_cast<int>(card.y + 44.0f), 11, ColorAlpha(text, 0.78f));
	}

	DrawPanel(preview, danger, {5, 8, 12, 232});
	DrawText("CURSE IN HAND", static_cast<int>(preview.x + 16.0f), static_cast<int>(preview.y + 13.0f), 17, danger);
	DrawText(spell.name().c_str(), static_cast<int>(preview.x + 16.0f), static_cast<int>(preview.y + 39.0f), width < 900 ? 17 : 21, text);
	DrawText(DeliveryText(spell.base()), static_cast<int>(preview.x + 16.0f), static_cast<int>(preview.y + 68.0f), 12, cyan);
	DrawLine(static_cast<int>(preview.x + 16.0f), static_cast<int>(preview.y + 91.0f),
		static_cast<int>(preview.x + preview.width - 16.0f), static_cast<int>(preview.y + 91.0f), dim);

	const SpellStats stats = spell.stats();
	float textY = preview.y + 104.0f;
	const int statSize = width < 900 ? 13 : 15;
	DrawText(TextFormat("DAMAGE     %.0f", stats.damage), static_cast<int>(preview.x + 16.0f), static_cast<int>(textY), statSize, text); textY += 22.0f;
	if (stats.heal > 0.0f) { DrawText(TextFormat("MEND       %.0f", stats.heal), static_cast<int>(preview.x + 16.0f), static_cast<int>(textY), statSize, text); textY += 22.0f; }
	if (stats.radius > 0.0f) { DrawText(TextFormat("REACH      %.1fm", stats.radius), static_cast<int>(preview.x + 16.0f), static_cast<int>(textY), statSize, text); textY += 22.0f; }
	if (stats.poisonDps > 0.0f) { DrawText(TextFormat("ROT        %.0f/s for %.1fs", stats.poisonDps, stats.duration), static_cast<int>(preview.x + 16.0f), static_cast<int>(textY), statSize, {179, 255, 68, 255}); textY += 22.0f; }
	if (stats.snareStrength > 0.0f) { DrawText(TextFormat("THORNS     %.0f%% slow", stats.snareStrength * 100.0f), static_cast<int>(preview.x + 16.0f), static_cast<int>(textY), statSize, {119, 255, 111, 255}); textY += 22.0f; }
	if (stats.fearDuration > 0.0f) { DrawText(TextFormat("PANIC      %.1fs", stats.fearDuration), static_cast<int>(preview.x + 16.0f), static_cast<int>(textY), statSize, {255, 108, 220, 255}); textY += 22.0f; }
	if (stats.wardGain > 0.0f) { DrawText(TextFormat("WARD RETURN +%.0f", stats.wardGain), static_cast<int>(preview.x + 16.0f), static_cast<int>(textY), statSize, {96, 228, 255, 255}); textY += 22.0f; }
	if (stats.homingStrength > 0.0f) { DrawText("SEEKING    YES", static_cast<int>(preview.x + 16.0f), static_cast<int>(textY), statSize, {255, 108, 220, 255}); textY += 22.0f; }
	if (stats.lingering) { DrawText("MIRE       LINGERS", static_cast<int>(preview.x + 16.0f), static_cast<int>(textY), statSize, {179, 255, 68, 255}); }

	const float slotY = preview.y + preview.height - 63.0f;
	DrawText("SLOTS", static_cast<int>(preview.x + 16.0f), static_cast<int>(slotY - 19.0f), 13, dim);
	for (std::size_t i = 0; i < CurseVector::MAX_REAGENTS; ++i) {
		const Rectangle slot {preview.x + 16.0f + static_cast<float>(i) * 58.0f, slotY, 50.0f, 34.0f};
		const bool filled = i < spell.reagents().size();
		const Color slotColor = filled ? GetReagentDefinition(spell.reagents()[i]).color : dim;
		DrawRectangleLinesEx(slot, 2.0f, slotColor);
		if (filled) {
			const char* label = GetReagentDefinition(spell.reagents()[i]).shortName.data();
			DrawText(label, static_cast<int>(slot.x + slot.width / 2.0f - MeasureText(label, 11) / 2), static_cast<int>(slot.y + 11.0f), 11, slotColor);
		}
	}
	DrawText("BACKSPACE UNDO  //  DEL CLEAR  //  RELEASE R TO CAST", static_cast<int>(left.x),
		static_cast<int>(left.y + left.height - 17.0f), 12, dim);
	if (!statusMessage_.empty()) {
		DrawText(statusMessage_.c_str(), static_cast<int>(preview.x + 16.0f), static_cast<int>(preview.y + preview.height - 22.0f), 12, danger);
	}
}

void CoatMenu::draw() const {
	if (drawPlayer_ != nullptr && drawSpell_ != nullptr) {
		draw(*drawPlayer_, *drawSpell_);
	}
}

void CoatMenu::reset() {
	active_ = false;
	statusMessage_.clear();
	statusTimer_ = 0.0f;
	drawPlayer_ = nullptr;
	drawSpell_ = nullptr;
}

void CoatMenu::clampSelectionToInventory(const Player& player, CurseVector& spell) {
	while (!spell.canConsume(player.inventory()) && spell.removeLastReagent()) {
	}
}

float CoatMenu::movementMultiplier() const {
	return active_ ? 0.5f : 1.0f;
}

const std::string& CoatMenu::statusMessage() const {
	return statusMessage_;
}

void CoatMenu::setStatus(std::string message) {
	statusMessage_ = std::move(message);
	statusTimer_ = 1.6f;
}
