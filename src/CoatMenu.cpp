#include "CoatMenu.h"

#include <algorithm>
#include <array>
#include <optional>
#include <string>

#include "Player.hxx"

namespace {
struct CoatLayout {
	Rectangle panel {};
	std::array<Rectangle, 4> baseButtons {};
	std::array<Rectangle, REAGENT_TYPE_COUNT> reagentButtons {};
	std::array<Rectangle, CurseVector::MAX_REAGENTS> ingredientSlots {};
	Rectangle removeButton {};
	Rectangle clearButton {};
	Rectangle castButton {};
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
	for (std::size_t index = 0; index < REAGENT_TYPE_COUNT; ++index) {
		const int column = static_cast<int>(index % 3);
		const int row = static_cast<int>(index / 3);
		layout.reagentButtons[index] = Rectangle {
			x + 18.0f + static_cast<float>(column) * (reagentWidth + 6.0f),
			y + 150.0f + static_cast<float>(row) * 68.0f, reagentWidth, 61.0f
		};
	}

	for (std::size_t index = 0; index < CurseVector::MAX_REAGENTS; ++index) {
		layout.ingredientSlots[index] = Rectangle {
			x + 18.0f + static_cast<float>(index) * 116.0f, y + 306.0f, 106.0f, 42.0f
		};
	}
	layout.removeButton = Rectangle {x + width - 221.0f, y + 306.0f, 92.0f, 42.0f};
	layout.clearButton = Rectangle {x + width - 121.0f, y + 306.0f, 103.0f, 42.0f};
	layout.castButton = Rectangle {x + width - 205.0f, y + height - 63.0f, 187.0f, 43.0f};
	return layout;
}

Color vectorColor(CurseVectorType type) {
	switch (type) {
	case CurseVectorType::Bottle: return Color {128, 226, 222, 255};
	case CurseVectorType::Apple: return Color {244, 190, 66, 255};
	case CurseVectorType::Spider: return Color {139, 239, 100, 255};
	case CurseVectorType::Poppet: return Color {195, 128, 238, 255};
	}
	return RAYWHITE;
}

void drawButton(Rectangle bounds, Color border, bool selected, bool disabled,
	const char* label, const char* detail = nullptr) {
	const Color fill = selected ? Color {border.r, border.g, border.b, 52} : Color {9, 16, 25, 218};
	DrawRectangleRec(bounds, fill);
	DrawRectangleLinesEx(bounds, selected ? 2.4f : 1.2f,
		disabled ? Color {81, 90, 100, 255} : border);
	const Color textColor = disabled ? Color {119, 127, 137, 255} : RAYWHITE;
	DrawText(label, static_cast<int>(bounds.x) + 9, static_cast<int>(bounds.y) + 8, 16, textColor);
	if (detail != nullptr) {
		DrawText(detail, static_cast<int>(bounds.x) + 9, static_cast<int>(bounds.y) + 29, 12,
			disabled ? Color {100, 108, 118, 255} : border);
	}
}

void drawReagentGlyph(Rectangle bounds, ReagentType type, Color color) {
	return;


	const Vector2 centre {bounds.x + bounds.width - 22.0f, bounds.y + bounds.height * 0.5f};
	switch (type) {
	case ReagentType::BoneShards:
		DrawRectangleLinesEx(Rectangle {centre.x - 8.0f, centre.y - 8.0f, 16.0f, 16.0f}, 1.4f, color);
		break;
	case ReagentType::WidowNettle:
		for (int thorn = 0; thorn < 4; ++thorn) {
			const float thornIndex = static_cast<float>(thorn);
			const float row = static_cast<float>(thorn % 2);
			DrawLineEx(centre, Vector2 {centre.x - 10.0f + thornIndex * 6.5f,
				centre.y - 11.0f + row * 22.0f}, 1.5f, color);
		}
		break;
	case ReagentType::Rotcap:
		DrawCircleLines(static_cast<int>(centre.x), static_cast<int>(centre.y), 9.0f, color);
		DrawCircleV(centre, 2.0f, color);
		break;
	case ReagentType::BatWing:
		DrawLineEx(Vector2 {centre.x - 11.0f, centre.y - 7.0f}, centre, 1.5f, color);
		DrawLineEx(centre, Vector2 {centre.x + 11.0f, centre.y - 7.0f}, 1.5f, color);
		DrawLineEx(Vector2 {centre.x - 11.0f, centre.y + 7.0f}, centre, 1.5f, color);
		DrawLineEx(centre, Vector2 {centre.x + 11.0f, centre.y + 7.0f}, 1.5f, color);
		break;
	case ReagentType::WitchGlass:
		DrawTriangleLines(Vector2 {centre.x, centre.y - 10.0f}, Vector2 {centre.x - 9.0f, centre.y + 8.0f},
			Vector2 {centre.x + 9.0f, centre.y + 8.0f}, color);
		break;
	case ReagentType::PreservedEye:
		DrawCircleLines(static_cast<int>(centre.x), static_cast<int>(centre.y + 3.0f), 8.0f, color);
		DrawCircleV(centre, 2.5f, color);
		break;
	case ReagentType::Count:
		break;
	}
}

const char* vectorDelivery(CurseVectorType type) {
	switch (type) {
	case CurseVectorType::Bottle: return "throw / impact curse";
	case CurseVectorType::Apple: return "eat / self pulse";
	case CurseVectorType::Spider: return "place / trap curse";
	case CurseVectorType::Poppet: return "free fallback needle";
	}
	return "free fallback needle";
}

void retainAffordableDoses(CurseVector& curse, const ReagentInventory& inventory) {
	if (curse.canAfford(inventory)) return;
	const CurseVector original = curse;
	curse = CurseVector(original.getType());
	for (int index = 0; index < original.getReagentCount(); ++index) {
		curse.addReagent(original.getReagent(index), inventory);
	}
}
}

CoatMenu::CoatMenu() : draft(CurseVectorType::Poppet) {
	setActive(false);
}

void CoatMenu::reset() {
	draft = CurseVector(CurseVectorType::Poppet);
	toggledOpen = false;
	committed = false;
	castRequested = false;
	statusLine = "Choose a base, then stitch up to three stacks.";
	setActive(false);
}

void CoatMenu::update(const InputHandler::InputState& input, Player& player) {
	castRequested = false;
	if (input.coatTogglePressed) {
		toggledOpen = !toggledOpen;
	}
	const bool shouldOpen = input.coatHeld || toggledOpen;
	if (shouldOpen && !active) {
		draft = player.getPreparedSpell();
		retainAffordableDoses(draft, player.getInventory());
		setActive(true);
		statusLine = "The world is live. Stitch quickly.";
	}
	if (!shouldOpen) {
		if (!active) return;
		retainAffordableDoses(draft, player.getInventory());
		player.setPreparedSpell(draft);
		committed = true;
		setActive(false);
		return;
	}

	handleSelection(input, player);
}

void CoatMenu::handleSelection(const InputHandler::InputState& input, Player& player) {
	static constexpr std::array<CurseVectorType, 4> kBaseTypes {
		CurseVectorType::Bottle, CurseVectorType::Apple, CurseVectorType::Spider, CurseVectorType::Poppet
	};

	if (input.coatVectorSelection >= 0
		&& input.coatVectorSelection < static_cast<int>(kBaseTypes.size())) {
		setBase(kBaseTypes[static_cast<std::size_t>(input.coatVectorSelection)]);
	}
	if (input.mainVectorPreviousPressed) {
		const auto current = std::find(kBaseTypes.begin(), kBaseTypes.end(), draft.getType());
		const std::size_t index = current == kBaseTypes.end()
			? 0U : static_cast<std::size_t>(std::distance(kBaseTypes.begin(), current));
		setBase(kBaseTypes[(index + kBaseTypes.size() - 1U) % kBaseTypes.size()]);
	}
	if (input.mainVectorNextPressed) {
		const auto current = std::find(kBaseTypes.begin(), kBaseTypes.end(), draft.getType());
		const std::size_t index = current == kBaseTypes.end()
			? 0U : static_cast<std::size_t>(std::distance(kBaseTypes.begin(), current));
		setBase(kBaseTypes[(index + 1U) % kBaseTypes.size()]);
	}

	if (input.coatUndoPressed && draft.removeLastReagent()) {
		statusLine = "Removed the last reagent.";
	}
	if (input.coatClearPressed) {
		draft.clearReagents();
		statusLine = "Reagent slots cleared.";
	}

	bool handledReagentKey = false;
	for (std::size_t index = 0; index < input.coatReagentPressed.size(); ++index) {
		if (!input.coatReagentPressed[index]) continue;
		handledReagentKey = true;
		if (input.coatRemoveHeld) {
			if (draft.removeReagent(static_cast<ReagentType>(index))) statusLine = "Removed a stitched reagent.";
		} else {
			tryAddReagent(static_cast<ReagentType>(index), player.getInventory());
		}
	}
	if (!handledReagentKey && input.coatReagentSelection >= 0
		&& input.coatReagentSelection < static_cast<int>(REAGENT_TYPE_COUNT)) {
		const ReagentType type = static_cast<ReagentType>(input.coatReagentSelection);
		if (input.coatRemoveHeld) {
			if (draft.removeReagent(type)) statusLine = "Removed a stitched reagent.";
		} else {
			tryAddReagent(type, player.getInventory());
		}
	}

	const CoatLayout layout = makeLayout(GetScreenWidth(), GetScreenHeight());
	if (input.menuClickPressed) {
		for (std::size_t index = 0; index < layout.baseButtons.size(); ++index) {
			if (CheckCollisionPointRec(input.mousePosition, layout.baseButtons[index])) {
				setBase(kBaseTypes[index]);
				return;
			}
		}
		for (std::size_t index = 0; index < layout.reagentButtons.size(); ++index) {
			if (CheckCollisionPointRec(input.mousePosition, layout.reagentButtons[index])) {
				tryAddReagent(static_cast<ReagentType>(index), player.getInventory());
				return;
			}
		}
		if (CheckCollisionPointRec(input.mousePosition, layout.removeButton)) {
			if (draft.removeLastReagent()) statusLine = "Removed the last reagent.";
			return;
		}
		if (CheckCollisionPointRec(input.mousePosition, layout.clearButton)) {
			draft.clearReagents();
			statusLine = "Reagent slots cleared.";
			return;
		}
		if (CheckCollisionPointRec(input.mousePosition, layout.castButton)) {
			if (draft.canAfford(player.getInventory())) {
				player.setPreparedSpell(draft);
				castRequested = true;
			} else {
				statusLine = "You need more of a selected stack.";
			}
			return;
		}
	}

	if (input.coatCastPressed) {
		if (draft.canAfford(player.getInventory())) {
			player.setPreparedSpell(draft);
			castRequested = true;
		} else {
			statusLine = "You need more of a selected stack.";
		}
	}
}

void CoatMenu::draw(const Player& player, const GameAssets& assets) const {
	(void)assets;
	if (!active) return;

	static constexpr std::array<CurseVectorType, 4> kBaseTypes {
		CurseVectorType::Bottle, CurseVectorType::Apple, CurseVectorType::Spider, CurseVectorType::Poppet
	};
	static constexpr std::array<const char*, 4> kBaseLabels {
		"1 BOTTLE", "2 APPLE", "3 SPIDER", "4 POPPET"
	};
	static constexpr std::array<const char*, REAGENT_TYPE_COUNT> kReagentKeys {
		"Z", "X", "C", "V", "B", "N"
	};

	const CoatLayout layout = makeLayout(GetScreenWidth(), GetScreenHeight());
	const Color line {141, 245, 205, 255};
	const Color warning {255, 134, 114, 255};
	DrawRectangleRec(layout.panel, Color {4, 10, 17, 235});
	DrawRectangleLinesEx(layout.panel, 2.0f, line);
	DrawText("COAT RUMMAGE", static_cast<int>(layout.panel.x) + 18,
		static_cast<int>(layout.panel.y) + 15, 24, RAYWHITE);
	DrawText("LIVE  //  MOVE 44%  //  SPRINT + JUMP LOCKED  //  VULNERABLE",
		static_cast<int>(layout.panel.x) + 204, static_cast<int>(layout.panel.y) + 20, 13, warning);

	for (std::size_t index = 0; index < layout.baseButtons.size(); ++index) {
		const CurseVectorType type = kBaseTypes[index];
		drawButton(layout.baseButtons[index], vectorColor(type), draft.getType() == type, false,
			kBaseLabels[index], vectorDelivery(type));
	}

	DrawText("STITCHED POCKETS   [Z X C / V B N]", static_cast<int>(layout.panel.x) + 18,
		static_cast<int>(layout.panel.y) + 128, 16, line);
	const ReagentInventory& inventory = player.getInventory();
	for (std::size_t index = 0; index < REAGENT_TYPE_COUNT; ++index) {
		const ReagentType type = static_cast<ReagentType>(index);
		const ReagentDefinition& definition = getReagentDefinition(type);
		const int owned = inventory[index];
		const int selected = selectedCount(type);
		const int available = std::max(0, owned - selected);
		const bool unavailable = available <= 0;
		const std::string detail = std::string("[") + kReagentKeys[index] + "] "
			+ std::to_string(available) + "/" + std::to_string(owned) + " free  " + definition.effect;
		drawButton(layout.reagentButtons[index], definition.color, false, unavailable,
			definition.name, detail.c_str());
		drawReagentGlyph(layout.reagentButtons[index], type, definition.color);
	}

	DrawText("SEWN INTO THIS CURSE", static_cast<int>(layout.panel.x) + 18,
		static_cast<int>(layout.panel.y) + 282, 15, line);
	for (std::size_t index = 0; index < layout.ingredientSlots.size(); ++index) {
		if (index >= static_cast<std::size_t>(draft.getReagentCount())) {
			drawButton(layout.ingredientSlots[index], Color {75, 92, 106, 255}, false, false, "EMPTY");
			continue;
		}
		const ReagentType selected = draft.getReagent(static_cast<int>(index));
		const ReagentDefinition& definition = getReagentDefinition(selected);
		drawButton(layout.ingredientSlots[index], definition.color, true, false, definition.shortName);
	}
	drawButton(layout.removeButton, Color {238, 180, 87, 255}, false, false, "BACK", "remove last");
	drawButton(layout.clearButton, Color {242, 117, 112, 255}, false, false, "DELETE", "clear all");

	const SpellStats stats = draft.calculateStats();
	const int previewY = static_cast<int>(layout.panel.y) + 372;
	DrawText("HEX PREVIEW", static_cast<int>(layout.panel.x) + 18, previewY, 17, RAYWHITE);
	const std::string title = draft.getVectorName() + "  —  " + vectorDelivery(draft.getType());
	DrawText(title.c_str(), static_cast<int>(layout.panel.x) + 18, previewY + 25, 17, line);
	if (draft.getType() == CurseVectorType::Apple) {
		const std::string self = "VITALITY +" + std::to_string(static_cast<int>(stats.selfHeal))
			+ "   WARD +" + std::to_string(static_cast<int>(stats.wardGain))
			+ "   HASTE " + std::to_string(stats.hasteDuration).substr(0, 3) + "s";
		DrawText(self.c_str(), static_cast<int>(layout.panel.x) + 18, previewY + 49, 16, RAYWHITE);
	} else {
		const std::string outcome = "DMG " + std::to_string(static_cast<int>(stats.impactDamage))
			+ "   RADIUS " + std::to_string(stats.radius).substr(0, 3) + "m   WARD +"
			+ std::to_string(static_cast<int>(stats.wardGain));
		DrawText(outcome.c_str(), static_cast<int>(layout.panel.x) + 18, previewY + 49, 16, RAYWHITE);
	}
	const std::string details = "SNARE " + std::to_string(stats.snareDuration).substr(0, 3)
		+ "s  ROT " + std::to_string(stats.rotDuration).substr(0, 3)
		+ "s  FEAR " + std::to_string(stats.fearDuration).substr(0, 3)
		+ "s  SEEK " + std::to_string(static_cast<int>(stats.seekingStrength * 100.0f))
		+ "%  HAZARD " + std::to_string(stats.lingeringDuration).substr(0, 3) + "s";
	DrawText(details.c_str(), static_cast<int>(layout.panel.x) + 18, previewY + 72, 14,
		Color {183, 204, 205, 255});

	DrawText(statusLine.c_str(), static_cast<int>(layout.panel.x) + 18,
		static_cast<int>(layout.panel.y + layout.panel.height) - 43, 13,
		draft.canAfford(inventory) ? line : warning);
	drawButton(layout.castButton, draft.canAfford(inventory) ? line : warning, false,
		!draft.canAfford(inventory), "ENTER / CLICK: CAST",
		draft.canAfford(inventory) ? "ingredients ready" : "missing stacks");
	DrawText("BACKSPACE remove  |  DELETE clear  |  Q / E cycle vector  |  R / TAB close",
		static_cast<int>(layout.panel.x) + 18, static_cast<int>(layout.panel.y + layout.panel.height) - 20,
		12, Color {183, 204, 205, 255});

	const Vector2 mouse = GetMousePosition();
	DrawCircleLines(static_cast<int>(mouse.x), static_cast<int>(mouse.y), 7.0f, RAYWHITE);
	DrawLineEx(Vector2 {mouse.x - 10.0f, mouse.y}, Vector2 {mouse.x + 10.0f, mouse.y}, 1.0f, RAYWHITE);
	DrawLineEx(Vector2 {mouse.x, mouse.y - 10.0f}, Vector2 {mouse.x, mouse.y + 10.0f}, 1.0f, RAYWHITE);
}

const CurseVector& CoatMenu::getDraft() const {
	return draft;
}

bool CoatMenu::wasCommitted() {
	const bool result = committed;
	committed = false;
	return result;
}

bool CoatMenu::takeCastRequest() {
	const bool result = castRequested;
	castRequested = false;
	return result;
}

void CoatMenu::completeCast(Player& player) {
	draft.clearReagents();
	player.setPreparedSpell(draft);
	statusLine = "The curse tears free from the coat.";
}

void CoatMenu::setBase(CurseVectorType type) {
	draft.setType(type);
	if (type == CurseVectorType::Poppet) {
		statusLine = "Poppet is a bare, no-cost needle curse.";
	} else {
		statusLine = "Base set. Stitch up to three reagent stacks.";
	}
}

bool CoatMenu::tryAddReagent(ReagentType type, const ReagentInventory& inventory) {
	if (draft.getType() == CurseVectorType::Poppet) {
		draft.setType(CurseVectorType::Bottle);
		statusLine = "A reagent turns the poppet into a bottle curse.";
	}
	if (draft.getDoseCount(type) >= inventory[static_cast<std::size_t>(type)]) {
		statusLine = "That pocket has no spare stack.";
		return false;
	}
	if (!draft.addReagent(type, inventory)) {
		statusLine = "A curse carries exactly three reagent selections.";
		return false;
	}
	statusLine = "Stitched into the current curse.";
	return true;
}

int CoatMenu::selectedCount(ReagentType type) const {
	return draft.getDoseCount(type);
}
