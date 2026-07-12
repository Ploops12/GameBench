#include <array>
#include <cstdio>
#include <string>
#include <vector>
#include "CoatMenu.h"
#include "Player.hxx"

namespace {
	constexpr std::array<BaseVectorType, 4> kBaseKeyOrder {
		BaseVectorType::Bottle,
		BaseVectorType::Apple,
		BaseVectorType::Spider,
		BaseVectorType::Poppet
	};
}

void CoatMenu::update(Player& player, const InputHandler::InputState& input) {
	visible = input.rummageHeld;
	if (!visible) {
		return;
	}

	CurseVector& builder = player.spellBuilder();
	for (int i = 0; i < 4; ++i) {
		if (input.basePressed[i]) {
			builder.setBase(kBaseKeyOrder[i]);
		}
	}

	for (int i = 0; i < ReagentTypeCount; ++i) {
		if (input.reagentPressed[i]) {
			builder.addReagent(static_cast<ReagentType>(i), player.inventory());
		}
	}

	if (input.removeSelectionPressed) {
		builder.removeLast();
	}

	if (input.clearSpellPressed) {
		builder.clear();
	}
}

void CoatMenu::draw(const Player& player, int screenWidth, int screenHeight) const {
	if (!visible) {
		return;
	}

	const Rectangle panel {
		40.0f,
		120.0f,
		static_cast<float>(screenWidth - 80),
		static_cast<float>(screenHeight - 220)
	};

	DrawRectangleRec(panel, Color {7, 10, 15, 215});
	DrawRectangleLinesEx(panel, 2.0f, Color {135, 175, 215, 220});
	DrawText("COAT RUMMAGE", static_cast<int>(panel.x) + 24, static_cast<int>(panel.y) + 20, 28, Color {238, 241, 248, 255});
	DrawText("World stays live. Speed reduced. Sprint locked.", static_cast<int>(panel.x) + 24, static_cast<int>(panel.y) + 54, 20, Color {255, 184, 110, 255});
	DrawText("Vulnerable: no sprint, no fresh jump, hunters keep advancing.", static_cast<int>(panel.x) + 24, static_cast<int>(panel.y) + 78, 18, Color {255, 120, 120, 255});

	const CurseVector& builder = player.spellBuilder();
	const SpellPreview preview = player.previewSpell();
	const std::array<int, ReagentTypeCount>& inventory = player.inventory();
	const std::array<int, ReagentTypeCount> stacks = builder.getStacks();

	DrawText("Base vectors [1-4]", static_cast<int>(panel.x) + 24, static_cast<int>(panel.y) + 100, 22, Color {210, 220, 235, 255});
	for (int i = 0; i < 4; ++i) {
		const BaseVectorType base = kBaseKeyOrder[i];
		const int boxX = static_cast<int>(panel.x) + 24 + i * 170;
		const int boxY = static_cast<int>(panel.y) + 132;
		const bool selected = builder.getBase() == base;
		DrawRectangleLines(boxX, boxY, 150, 70, selected ? Color {255, 192, 108, 255} : Color {120, 150, 188, 180});
		std::string title = std::string("[") + static_cast<char>('1' + i) + "] " + GetBaseVectorName(static_cast<BaseVectorType>(i));
		title = std::string("[") + static_cast<char>('1' + i) + "] " + GetBaseVectorName(base);
		DrawText(title.c_str(), boxX + 12, boxY + 12, 22, Color {230, 238, 247, 255});
		DrawText(GetBaseVectorShortName(base), boxX + 12, boxY + 40, 18, Color {180, 198, 220, 255});
	}

	DrawText("Selected ingredients", static_cast<int>(panel.x) + 720, static_cast<int>(panel.y) + 100, 24, Color {230, 238, 247, 255});
	for (int slot = 0; slot < 3; ++slot) {
		const int slotX = static_cast<int>(panel.x) + 720 + slot * 135;
		const int slotY = static_cast<int>(panel.y) + 132;
		DrawRectangleLines(slotX, slotY, 120, 70, Color {125, 162, 202, 180});
		if (slot < builder.selectionCount()) {
			const ReagentDescriptor& descriptor = GetReagentDescriptor(builder.getSelections()[slot]);
			DrawText(descriptor.shortName, slotX + 12, slotY + 12, 22, descriptor.color);
			DrawText(descriptor.effectLabel, slotX + 12, slotY + 40, 18, Color {205, 215, 232, 255});
		} else {
			DrawText("Empty", slotX + 26, slotY + 24, 20, Color {105, 125, 150, 255});
		}
	}

	DrawText("Reagents [Q/W/E/A/S/D]", static_cast<int>(panel.x) + 24, static_cast<int>(panel.y) + 230, 22, Color {210, 220, 235, 255});
	for (int i = 0; i < ReagentTypeCount; ++i) {
		const ReagentDescriptor& descriptor = GetReagentDescriptor(static_cast<std::size_t>(i));
		const int col = i % 3;
		const int row = i / 3;
		const int boxX = static_cast<int>(panel.x) + 24 + col * 220;
		const int boxY = static_cast<int>(panel.y) + 262 + row * 92;
		DrawRectangleLines(boxX, boxY, 200, 74, descriptor.color);
		std::string key = std::string("[") + static_cast<char>(descriptor.key) + "]";
		DrawText(key.c_str(), boxX + 10, boxY + 10, 20, descriptor.color);
		DrawText(descriptor.name, boxX + 48, boxY + 10, 22, Color {230, 238, 247, 255});

		char count[64];
		std::snprintf(count, sizeof(count), "held %d   selected %d", inventory[i], stacks[i]);
		DrawText(count, boxX + 12, boxY + 40, 18, Color {185, 198, 219, 255});
		DrawText(descriptor.effectLabel, boxX + 132, boxY + 40, 18, descriptor.color);
	}

	DrawText("Preview", static_cast<int>(panel.x) + 720, static_cast<int>(panel.y) + 230, 24, Color {230, 238, 247, 255});
	DrawRectangleLines(static_cast<int>(panel.x) + 720, static_cast<int>(panel.y) + 262, 420, 220, Color {125, 162, 202, 180});

	std::vector<std::string> lines = DescribeSpell(preview);
	for (std::size_t i = 0; i < lines.size() && i < 9; ++i) {
		DrawText(lines[i].c_str(), static_cast<int>(panel.x) + 740, static_cast<int>(panel.y) + 286 + static_cast<int>(i) * 26, 20, Color {210, 220, 235, 255});
	}

	DrawText("Backspace/X remove last   C clear", static_cast<int>(panel.x) + 720, static_cast<int>(panel.y) + 500, 20, Color {180, 198, 220, 255});
	DrawText("Cast at any time with LMB", static_cast<int>(panel.x) + 720, static_cast<int>(panel.y) + 530, 20, Color {255, 184, 110, 255});
}
