#include "CoatMenu.h"

#include <algorithm>
#include <array>
#include <string_view>

#include <raylib.h>

namespace {
struct CoatLayout {
	Rectangle panel;
	std::array<Rectangle, 4> baseButtons;
	std::array<Rectangle, kReagentTypeCount> reagentButtons;
	std::array<Rectangle, CurseVector::kMaxReagents> ingredientSlots;
	Rectangle removeButton;
	Rectangle clearButton;
	Rectangle castButton;
};

CoatLayout makeLayout(int screenWidth, int screenHeight) {
	const float width = std::min(680.0f, static_cast<float>(screenWidth) - 56.0f);
	const float height = std::min(550.0f, static_cast<float>(screenHeight) - 70.0f);
	const float x = (static_cast<float>(screenWidth) - width) * 0.5f;
	const float y = 34.0f;
	CoatLayout layout {};
	layout.panel = Rectangle {x, y, width, height};

	const float baseWidth = (width - 58.0f) / 4.0f;
	for (int index = 0; index < 4; ++index) {
		layout.baseButtons[static_cast<std::size_t>(index)] = Rectangle {
			x + 18.0f + static_cast<float>(index) * (baseWidth + 6.0f), y + 59.0f, baseWidth, 54.0f
		};
	}

	const float reagentWidth = (width - 54.0f) / 3.0f;
	for (std::size_t index = 0; index < kReagentTypeCount; ++index) {
		const int column = static_cast<int>(index % 3);
		const int row = static_cast<int>(index / 3);
		layout.reagentButtons[index] = Rectangle {
			x + 18.0f + static_cast<float>(column) * (reagentWidth + 6.0f),
			y + 150.0f + static_cast<float>(row) * 68.0f, reagentWidth, 61.0f
		};
	}

	for (std::size_t index = 0; index < CurseVector::kMaxReagents; ++index) {
		layout.ingredientSlots[index] = Rectangle {
			x + 18.0f + static_cast<float>(index) * 116.0f, y + 306.0f, 106.0f, 42.0f
		};
	}
	layout.removeButton = Rectangle {x + width - 221.0f, y + 306.0f, 92.0f, 42.0f};
	layout.clearButton = Rectangle {x + width - 121.0f, y + 306.0f, 103.0f, 42.0f};
	layout.castButton = Rectangle {x + width - 205.0f, y + height - 63.0f, 187.0f, 43.0f};
	return layout;
}

Color reagentColor(ReagentType type) {
	switch (type) {
	case ReagentType::GraveSalt:
		return Color {196, 240, 231, 255};
	case ReagentType::ChurchGlass:
		return Color {74, 207, 225, 255};
	case ReagentType::WidowNettle:
		return Color {139, 239, 100, 255};
	case ReagentType::PlagueHoney:
		return Color {244, 190, 66, 255};
	case ReagentType::MothDust:
		return Color {195, 128, 238, 255};
	case ReagentType::BlackWax:
		return Color {241, 92, 115, 255};
	case ReagentType::Count:
		break;
	}

	return RAYWHITE;
}

void drawButton(Rectangle bounds, Color border, bool selected, bool disabled,
	const char* label, const char* detail = nullptr) {
	const Color fill = selected ? Color {border.r, border.g, border.b, 52} : Color {9, 16, 25, 218};
	DrawRectangleRec(bounds, fill);
	DrawRectangleLinesEx(bounds, selected ? 2.4f : 1.2f, disabled ? Color {81, 90, 100, 255} : border);
	const Color textColor = disabled ? Color {119, 127, 137, 255} : RAYWHITE;
	DrawText(label, static_cast<int>(bounds.x) + 9, static_cast<int>(bounds.y) + 8, 16, textColor);
	if (detail != nullptr) {
		DrawText(detail, static_cast<int>(bounds.x) + 9, static_cast<int>(bounds.y) + 29, 12,
			disabled ? Color {100, 108, 118, 255} : border);
	}
}

void drawReagentGlyph(Rectangle bounds, ReagentType type, Color color) {
	const Vector2 centre {bounds.x + bounds.width - 22.0f, bounds.y + bounds.height * 0.5f};
	switch (type) {
	case ReagentType::GraveSalt:
		DrawRectangleLinesEx(Rectangle {centre.x - 8.0f, centre.y - 8.0f, 16.0f, 16.0f}, 1.4f, color);
		break;
	case ReagentType::ChurchGlass:
		DrawTriangleLines(Vector2 {centre.x, centre.y - 10.0f}, Vector2 {centre.x - 9.0f, centre.y + 8.0f},
			Vector2 {centre.x + 9.0f, centre.y + 8.0f}, color);
		break;
	case ReagentType::WidowNettle:
		for (int thorn = 0; thorn < 4; ++thorn) {
			const float thornIndex = static_cast<float>(thorn);
			const float row = static_cast<float>(thorn % 2);
			DrawLineEx(centre, Vector2 {centre.x - 10.0f + thornIndex * 6.5f, centre.y - 11.0f + row * 22.0f}, 1.5f, color);
		}
		break;
	case ReagentType::PlagueHoney:
		DrawCircleLines(static_cast<int>(centre.x), static_cast<int>(centre.y), 9.0f, color);
		DrawCircleV(centre, 2.0f, color);
		break;
	case ReagentType::MothDust:
		DrawLineEx(Vector2 {centre.x - 11.0f, centre.y - 7.0f}, centre, 1.5f, color);
		DrawLineEx(centre, Vector2 {centre.x + 11.0f, centre.y - 7.0f}, 1.5f, color);
		DrawLineEx(Vector2 {centre.x - 11.0f, centre.y + 7.0f}, centre, 1.5f, color);
		DrawLineEx(centre, Vector2 {centre.x + 11.0f, centre.y + 7.0f}, 1.5f, color);
		break;
	case ReagentType::BlackWax:
		DrawCircleLines(static_cast<int>(centre.x), static_cast<int>(centre.y + 3.0f), 8.0f, color);
		DrawLineEx(Vector2 {centre.x, centre.y - 11.0f}, Vector2 {centre.x, centre.y - 2.0f}, 1.5f, color);
		break;
	case ReagentType::Count:
		break;
	}
}
}

void CoatMenu::update(const InputHandler::InputState& input, const ReagentInventory& inventory,
	int screenWidth, int screenHeight) {
	castRequested = false;
	if (input.rummageTogglePressed) {
		toggledOpen = !toggledOpen;
	}
	open = input.rummageHeld || toggledOpen;
	if (!open) {
		return;
	}

	if (input.baseBottlePressed) {
		setBase(CurseVectorType::Bottle);
	}
	if (input.baseApplePressed) {
		setBase(CurseVectorType::Apple);
	}
	if (input.baseSpiderPressed) {
		setBase(CurseVectorType::Spider);
	}
	if (input.basePoppetPressed) {
		setBase(CurseVectorType::Poppet);
	}

	for (std::size_t index = 0; index < kReagentTypeCount; ++index) {
		if (input.reagentPressed[index]) {
			tryAddReagent(static_cast<ReagentType>(index), inventory);
		}
	}
	if (input.removeReagentPressed && curseVector.removeLastReagent()) {
		statusLine = "Removed the last reagent.";
	}
	if (input.clearRecipePressed) {
		curseVector.clearReagents();
		statusLine = "Reagent slots cleared.";
	}

	const CoatLayout layout = makeLayout(screenWidth, screenHeight);
	if (!input.castPressed) {
		if (input.menuCastPressed) {
			castRequested = true;
		}
		return;
	}

	const Vector2 mouse = input.mousePosition;
	for (std::size_t index = 0; index < layout.baseButtons.size(); ++index) {
		if (CheckCollisionPointRec(mouse, layout.baseButtons[index])) {
			setBase(static_cast<CurseVectorType>(index));
			return;
		}
	}
	for (std::size_t index = 0; index < layout.reagentButtons.size(); ++index) {
		if (CheckCollisionPointRec(mouse, layout.reagentButtons[index])) {
			tryAddReagent(static_cast<ReagentType>(index), inventory);
			return;
		}
	}
	if (CheckCollisionPointRec(mouse, layout.removeButton)) {
		if (curseVector.removeLastReagent()) {
			statusLine = "Removed the last reagent.";
		}
		return;
	}
	if (CheckCollisionPointRec(mouse, layout.clearButton)) {
		curseVector.clearReagents();
		statusLine = "Reagent slots cleared.";
		return;
	}
	if (CheckCollisionPointRec(mouse, layout.castButton)) {
		castRequested = true;
	}
}

void CoatMenu::draw(const ReagentInventory& inventory, int screenWidth, int screenHeight) const {
	if (!open) {
		return;
	}

	const CoatLayout layout = makeLayout(screenWidth, screenHeight);
	const Color line {141, 245, 205, 255};
	const Color warning {255, 134, 114, 255};
	DrawRectangleRec(layout.panel, Color {4, 10, 17, 235});
	DrawRectangleLinesEx(layout.panel, 2.0f, line);
	DrawText("COAT RUMMAGE", static_cast<int>(layout.panel.x) + 18, static_cast<int>(layout.panel.y) + 15, 24, RAYWHITE);
	DrawText("LIVE  //  MOVE 44%  //  SPRINT LOCKED  //  VULNERABLE", static_cast<int>(layout.panel.x) + 204,
		static_cast<int>(layout.panel.y) + 20, 13, warning);

	static constexpr std::array<const char*, 4> kBaseLabels {"1 BOTTLE", "2 APPLE", "3 SPIDER", "4 POPPET"};
	for (std::size_t index = 0; index < layout.baseButtons.size(); ++index) {
		const CurseVectorType type = static_cast<CurseVectorType>(index);
		drawButton(layout.baseButtons[index], Color {128, 226, 222, 255}, curseVector.base() == type, false,
			kBaseLabels[index], curseVectorDelivery(type).data());
	}

	DrawText("STITCHED POCKETS   [Z X C / V B N]", static_cast<int>(layout.panel.x) + 18,
		static_cast<int>(layout.panel.y) + 128, 16, line);
	static constexpr std::array<const char*, kReagentTypeCount> kKeys {"Z", "X", "C", "V", "B", "N"};
	for (std::size_t index = 0; index < kReagentTypeCount; ++index) {
		const ReagentType type = static_cast<ReagentType>(index);
		const ReagentDefinition& definition = Reagent::definition(type);
		const int owned = inventory.count(type);
		const int selected = selectedCount(type);
		const int available = std::max(0, owned - selected);
		const bool unavailable = available <= 0;
		char detail[96] {};
		TextCopy(detail, TextFormat("[%s] %d/%d free  %s", kKeys[index], available, owned, definition.effect.data()));
		drawButton(layout.reagentButtons[index], reagentColor(type), false, unavailable, definition.name.data(), detail);
		drawReagentGlyph(layout.reagentButtons[index], type, reagentColor(type));
	}

	DrawText("SEWN INTO THIS CURSE", static_cast<int>(layout.panel.x) + 18, static_cast<int>(layout.panel.y) + 282, 15, line);
	for (std::size_t index = 0; index < layout.ingredientSlots.size(); ++index) {
		const std::optional<ReagentType> selected = curseVector.reagentAt(index);
		if (!selected.has_value()) {
			drawButton(layout.ingredientSlots[index], Color {75, 92, 106, 255}, false, false, "EMPTY");
			continue;
		}

		const ReagentDefinition& definition = Reagent::definition(*selected);
		drawButton(layout.ingredientSlots[index], reagentColor(*selected), true, false, definition.name.data());
	}
	drawButton(layout.removeButton, Color {238, 180, 87, 255}, false, false, "BACK", "remove last");
	drawButton(layout.clearButton, Color {242, 117, 112, 255}, false, false, "DELETE", "clear all");

	const SpellStats stats = curseVector.stats();
	const int previewY = static_cast<int>(layout.panel.y) + 372;
	DrawText("HEX PREVIEW", static_cast<int>(layout.panel.x) + 18, previewY, 17, RAYWHITE);
	DrawText(TextFormat("%s  —  %s", curseVectorName(curseVector.base()).data(), curseVectorDelivery(curseVector.base()).data()),
		static_cast<int>(layout.panel.x) + 18, previewY + 25, 17, line);
	if (curseVector.base() == CurseVectorType::Apple) {
		const SelfHexStats self = selfHexStats(stats);
		DrawText(TextFormat("VITALITY +%.0f   WARD +%.0f   THORNS %.1fs", self.healthRestore, self.wardGain,
			self.thornGuardDuration), static_cast<int>(layout.panel.x) + 18, previewY + 49, 16, RAYWHITE);
		DrawText(TextFormat("MEND %.1fs  HASTE %.1fs  WAX SHELL %.1fs", self.mendDuration,
			self.hasteDuration, self.waxShellDuration), static_cast<int>(layout.panel.x) + 18, previewY + 72, 14,
			Color {183, 204, 205, 255});
	} else {
		DrawText(TextFormat("DMG %.0f   RADIUS %.1fm   WARD +%.0f", stats.damage, stats.radius, stats.wardGain),
			static_cast<int>(layout.panel.x) + 18, previewY + 49, 16, RAYWHITE);
		DrawText(TextFormat("SNARE %.1fs  ROT %.1fs  FEAR %.1fs  SEEK %.0f%%  HAZARD %.1fs",
			stats.snareDuration, stats.poisonDuration, stats.fearDuration, stats.homingStrength * 100.0f, stats.lingeringDuration),
			static_cast<int>(layout.panel.x) + 18, previewY + 72, 14, Color {183, 204, 205, 255});
	}
	DrawText(statusLine.c_str(), static_cast<int>(layout.panel.x) + 18, static_cast<int>(layout.panel.y + layout.panel.height) - 43,
		13, curseVector.canConsume(inventory) ? line : warning);
	drawButton(layout.castButton, curseVector.canConsume(inventory) ? line : warning, false, !curseVector.canConsume(inventory),
		"ENTER / CLICK: CAST", curseVector.canConsume(inventory) ? "ingredients ready" : "missing stacks");

	const Vector2 mouse = GetMousePosition();
	DrawCircleLines(static_cast<int>(mouse.x), static_cast<int>(mouse.y), 7.0f, RAYWHITE);
	DrawLineEx(Vector2 {mouse.x - 10.0f, mouse.y}, Vector2 {mouse.x + 10.0f, mouse.y}, 1.0f, RAYWHITE);
	DrawLineEx(Vector2 {mouse.x, mouse.y - 10.0f}, Vector2 {mouse.x, mouse.y + 10.0f}, 1.0f, RAYWHITE);
}

bool CoatMenu::isOpen() const {
	return open;
}

bool CoatMenu::takeCastRequest() {
	const bool requested = castRequested;
	castRequested = false;
	return requested;
}

bool CoatMenu::consumeForCast(ReagentInventory& inventory) {
	if (!curseVector.consume(inventory)) {
		statusLine = "You need more of a selected stack.";
		return false;
	}

	if (!curseVector.empty()) {
		curseVector.clearReagents();
	}
	statusLine = "The curse tears free from the coat.";
	return true;
}

const CurseVector& CoatMenu::getCurseVector() const {
	return curseVector;
}

CurseVector& CoatMenu::getCurseVector() {
	return curseVector;
}

void CoatMenu::setBase(CurseVectorType base) {
	curseVector.setBase(base);
	if (base == CurseVectorType::Poppet) {
		curseVector.clearReagents();
		statusLine = "Poppet is a bare, no-cost needle curse.";
	} else {
		statusLine = "Base set. Stitch up to three reagent stacks.";
	}
}

bool CoatMenu::tryAddReagent(ReagentType type, const ReagentInventory& inventory) {
	if (curseVector.base() == CurseVectorType::Poppet) {
		curseVector.setBase(CurseVectorType::Bottle);
		statusLine = "A reagent turns the poppet into a bottle curse.";
	}
	if (selectedCount(type) >= inventory.count(type)) {
		statusLine = "That pocket has no spare stack.";
		return false;
	}
	if (!curseVector.addReagent(type)) {
		statusLine = "A curse carries exactly three reagent selections.";
		return false;
	}

	statusLine = "Stitched into the current curse.";
	return true;
}

int CoatMenu::selectedCount(ReagentType type) const {
	int count = 0;
	for (const ReagentType selected : curseVector.reagents()) {
		if (selected == type) {
			++count;
		}
	}
	return count;
}
