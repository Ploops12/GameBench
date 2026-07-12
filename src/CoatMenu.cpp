#include "CoatMenu.h"

#include <algorithm>
#include <array>
#include <string>

#include "Player.hxx"

namespace {
constexpr Color INK {224, 231, 220, 255};
constexpr Color DIM_INK {142, 153, 144, 255};
constexpr Color PANEL {8, 12, 13, 236};
constexpr Color LIVE {237, 107, 79, 255};
constexpr Color SELECTED {245, 202, 92, 255};

bool textureReady(const Texture2D& texture) {
	return texture.id != 0 && texture.width > 0 && texture.height > 0;
}

Rectangle firstFrameSource(const Texture2D& texture) {
	float width = static_cast<float>(texture.width);
	if (texture.width > texture.height * 2) width /= 8.0f;
	return Rectangle {0.0f, 0.0f, width, static_cast<float>(texture.height)};
}

void drawTextureFit(const Texture2D& texture, Rectangle bounds, Color tint = WHITE) {
	if (!textureReady(texture)) return;
	const Rectangle source = firstFrameSource(texture);
	const float scale = std::min(bounds.width / source.width, bounds.height / source.height);
	const Rectangle destination {
		bounds.x + (bounds.width - source.width * scale) * 0.5f,
		bounds.y + (bounds.height - source.height * scale) * 0.5f,
		source.width * scale,
		source.height * scale
	};
	DrawTexturePro(texture, source, destination, Vector2 {}, 0.0f, tint);
}

void drawVectorGlyph(CurseVectorType type, Vector2 centre, float size, Color color) {
	switch (type) {
		case CurseVectorType::Bottle:
			DrawRectangleLinesEx(Rectangle {centre.x - size * 0.22f, centre.y - size * 0.2f,
				size * 0.44f, size * 0.55f}, 2.0f, color);
			DrawRectangleLinesEx(Rectangle {centre.x - size * 0.1f, centre.y - size * 0.35f,
				size * 0.2f, size * 0.16f}, 2.0f, color);
			break;
		case CurseVectorType::Apple:
			DrawCircleLines(static_cast<int>(centre.x), static_cast<int>(centre.y), size * 0.28f, color);
			DrawLineEx(centre, Vector2 {centre.x + size * 0.08f, centre.y - size * 0.36f}, 2.0f, color);
			DrawLineEx(Vector2 {centre.x + size * 0.07f, centre.y - size * 0.3f},
				Vector2 {centre.x + size * 0.24f, centre.y - size * 0.26f}, 2.0f, color);
			break;
		case CurseVectorType::Spider:
			DrawCircleLines(static_cast<int>(centre.x), static_cast<int>(centre.y), size * 0.19f, color);
			for (int side : {-1, 1}) {
				for (int leg = -1; leg <= 2; ++leg) {
					const float y = centre.y + (leg - 0.5f) * size * 0.12f;
					DrawLineEx(Vector2 {centre.x + side * size * 0.14f, y},
						Vector2 {centre.x + side * size * 0.36f, y + (leg - 0.5f) * size * 0.06f},
						2.0f, color);
				}
			}
			break;
		case CurseVectorType::Poppet:
			DrawCircleLines(static_cast<int>(centre.x), static_cast<int>(centre.y - size * 0.2f),
				size * 0.11f, color);
			DrawLineEx(Vector2 {centre.x, centre.y - size * 0.09f},
				Vector2 {centre.x, centre.y + size * 0.3f}, 2.0f, color);
			DrawLineEx(Vector2 {centre.x - size * 0.25f, centre.y + size * 0.02f},
				Vector2 {centre.x + size * 0.25f, centre.y + size * 0.02f}, 2.0f, color);
			break;
	}
}

const Texture2D* vectorTexture(CurseVectorType type, const GameAssets& assets) {
	switch (type) {
		case CurseVectorType::Bottle: return &assets.vectorIcons[0];
		case CurseVectorType::Apple: return &assets.vectorIcons[1];
		case CurseVectorType::Spider: return &assets.vectorIcons[2];
		case CurseVectorType::Poppet: return &assets.poppetIcon;
	}
	return nullptr;
}

void drawSectionLabel(const char* text, int x, int y) {
	DrawText(text, x, y, 14, DIM_INK);
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
	committed = false;
	setActive(false);
}

void CoatMenu::update(const InputHandler::InputState& input, Player& player) {
	if (input.coatHeld) {
		if (!active) {
			draft = player.getPreparedSpell();
			retainAffordableDoses(draft, player.getInventory());
			setActive(true);
		}
		handleSelection(input, player);
		return;
	}

	if (!active) return;
	retainAffordableDoses(draft, player.getInventory());
	player.setPreparedSpell(draft);
	committed = true;
	setActive(false);
}

void CoatMenu::handleSelection(const InputHandler::InputState& input, Player& player) {
	switch (input.coatVectorSelection) {
	case 0: draft.setType(CurseVectorType::Bottle); break;
	case 1: draft.setType(CurseVectorType::Apple); break;
	case 2: draft.setType(CurseVectorType::Spider); break;
	case 3: draft.setType(CurseVectorType::Poppet); break;
	default: break;
	}

	if (input.coatUndoPressed) draft.removeLastReagent();
	if (input.coatClearPressed) draft.clearReagents();

	if (input.coatReagentSelection >= 0
			&& input.coatReagentSelection < static_cast<int>(REAGENT_TYPE_COUNT)) {
		const ReagentType reagent = static_cast<ReagentType>(input.coatReagentSelection);
		if (input.coatRemoveHeld) draft.removeReagent(reagent);
		else draft.addReagent(reagent, player.getInventory());
	}
}

void CoatMenu::draw(const Player& player, const GameAssets& assets) const {
	if (!active) return;

	const int screenWidth = GetScreenWidth();
	const int screenHeight = GetScreenHeight();
	const Rectangle panel {32.0f, 54.0f, static_cast<float>(screenWidth - 64),
		static_cast<float>(screenHeight - 88)};
	DrawRectangleRec(panel, PANEL);
	DrawRectangleLinesEx(panel, 2.0f, INK);
	DrawLine(static_cast<int>(panel.x + 12.0f), static_cast<int>(panel.y + 38.0f),
		static_cast<int>(panel.x + panel.width - 12.0f), static_cast<int>(panel.y + 38.0f), DIM_INK);

	DrawText("COAT RUMMAGE // RELEASE TAB TO READY", static_cast<int>(panel.x + 14.0f),
		static_cast<int>(panel.y + 11.0f), 16, INK);
	const char* danger = "WORLD LIVE // SLOWED // SPRINT OFF // EXPOSED";
	const int dangerWidth = MeasureText(danger, 12);
	DrawText(danger, static_cast<int>(panel.x + panel.width - dangerWidth - 14.0f),
		static_cast<int>(panel.y + 14.0f), 12, LIVE);

	drawSectionLabel("BASE VECTOR", static_cast<int>(panel.x + 14.0f), static_cast<int>(panel.y + 50.0f));
	constexpr std::array<CurseVectorType, 4> types {
		CurseVectorType::Bottle, CurseVectorType::Apple, CurseVectorType::Spider, CurseVectorType::Poppet
	};
	constexpr std::array<const char*, 4> keys {"1", "2", "3", "0"};
	const float baseGap = 8.0f;
	const float baseWidth = (panel.width - 28.0f - baseGap * 3.0f) / 4.0f;
	const float baseY = panel.y + 69.0f;
	for (std::size_t i = 0; i < types.size(); ++i) {
		const Rectangle card {panel.x + 14.0f + i * (baseWidth + baseGap), baseY, baseWidth, 67.0f};
		const bool selected = draft.getType() == types[i];
		const Color border = selected ? SELECTED : DIM_INK;
		if (selected) DrawRectangleRec(card, Color {40, 37, 22, 220});
		DrawRectangleLinesEx(card, selected ? 2.0f : 1.0f, border);
		DrawText(keys[i], static_cast<int>(card.x + 7.0f), static_cast<int>(card.y + 6.0f), 16, border);
		const Texture2D* texture = vectorTexture(types[i], assets);
		const Rectangle iconBox {card.x + 26.0f, card.y + 5.0f, 42.0f, 40.0f};
		if (texture != nullptr && textureReady(*texture)) drawTextureFit(*texture, iconBox);
		else drawVectorGlyph(types[i], Vector2 {iconBox.x + 21.0f, iconBox.y + 20.0f}, 40.0f, border);
		CurseVector vector(types[i]);
		const std::string name = vector.getVectorName();
		DrawText(name.c_str(), static_cast<int>(card.x + 74.0f), static_cast<int>(card.y + 12.0f), 17, INK);
		const char* delivery = types[i] == CurseVectorType::Bottle ? "THROWN" :
			types[i] == CurseVectorType::Apple ? "SELF PULSE" :
			types[i] == CurseVectorType::Spider ? "PLACED MINE" : "FREE NEEDLE";
		DrawText(delivery, static_cast<int>(card.x + 74.0f), static_cast<int>(card.y + 37.0f), 11, DIM_INK);
	}

	drawSectionLabel("THREE DOSES MAX // PRESS 4-9 TO ADD // CTRL + 4-9 TO REMOVE",
		static_cast<int>(panel.x + 14.0f), static_cast<int>(panel.y + 151.0f));
	const ReagentInventory& inventory = player.getInventory();
	const float reagentGap = 6.0f;
	const float reagentWidth = (panel.width - 28.0f - reagentGap * 5.0f) / 6.0f;
	const float reagentY = panel.y + 170.0f;
	for (std::size_t i = 0; i < REAGENT_TYPE_COUNT; ++i) {
		const ReagentDefinition& definition = getReagentDefinitions()[i];
		const int doses = draft.getDoseCount(definition.type);
		const bool available = inventory[i] > doses && draft.getReagentCount() < CurseVector::MAX_REAGENTS
			&& draft.getType() != CurseVectorType::Poppet;
		const Rectangle card {panel.x + 14.0f + i * (reagentWidth + reagentGap), reagentY,
			reagentWidth, 91.0f};
		if (doses > 0) DrawRectangleRec(card, Color {24, 31, 29, 230});
		DrawRectangleLinesEx(card, doses > 0 ? 2.0f : 1.0f, doses > 0 ? definition.color : DIM_INK);
		const std::string key = std::to_string(definition.selectionKey - KEY_ZERO);
		DrawText(key.c_str(), static_cast<int>(card.x + 6.0f), static_cast<int>(card.y + 5.0f), 16,
			available ? INK : DIM_INK);
		const Rectangle iconBox {card.x + card.width - 37.0f, card.y + 4.0f, 30.0f, 30.0f};
		if (textureReady(assets.reagentIcons[i])) drawTextureFit(assets.reagentIcons[i], iconBox);
		else DrawCircleLines(static_cast<int>(iconBox.x + 15.0f), static_cast<int>(iconBox.y + 15.0f),
			10.0f, definition.color);
		DrawText(definition.shortName, static_cast<int>(card.x + 6.0f), static_cast<int>(card.y + 39.0f),
			12, definition.color);
		const std::string quantity = "COAT " + std::to_string(inventory[i]) + "  IN " + std::to_string(doses);
		DrawText(quantity.c_str(), static_cast<int>(card.x + 6.0f), static_cast<int>(card.y + 58.0f),
			11, available || doses > 0 ? INK : DIM_INK);
		DrawText(definition.effect, static_cast<int>(card.x + 6.0f), static_cast<int>(card.y + 75.0f),
			9, DIM_INK);
	}

	const float resultY = panel.y + 277.0f;
	drawSectionLabel("SELECTED DOSES", static_cast<int>(panel.x + 14.0f), static_cast<int>(resultY));
	for (int slot = 0; slot < CurseVector::MAX_REAGENTS; ++slot) {
		const Rectangle slotBox {panel.x + 14.0f + slot * 142.0f, resultY + 20.0f, 132.0f, 35.0f};
		DrawRectangleLinesEx(slotBox, 1.0f, slot < draft.getReagentCount() ? SELECTED : DIM_INK);
		if (slot < draft.getReagentCount()) {
			const ReagentDefinition& definition = getReagentDefinition(draft.getReagent(slot));
			DrawCircle(static_cast<int>(slotBox.x + 15.0f), static_cast<int>(slotBox.y + 17.0f), 6.0f,
				definition.color);
			DrawText(definition.shortName, static_cast<int>(slotBox.x + 29.0f),
				static_cast<int>(slotBox.y + 10.0f), 14, INK);
		} else {
			DrawText("EMPTY", static_cast<int>(slotBox.x + 34.0f), static_cast<int>(slotBox.y + 10.0f),
				13, DIM_INK);
		}
	}

	const Rectangle previewBox {panel.x + 14.0f, resultY + 69.0f, panel.width - 28.0f, 82.0f};
	DrawRectangleLinesEx(previewBox, 2.0f, SELECTED);
	DrawText("RESULT", static_cast<int>(previewBox.x + 10.0f), static_cast<int>(previewBox.y + 8.0f), 12,
		DIM_INK);
	const std::string curseName = draft.getCurseName();
	DrawText(curseName.c_str(), static_cast<int>(previewBox.x + 86.0f), static_cast<int>(previewBox.y + 6.0f),
		20, SELECTED);
	const std::string preview = draft.getPreview();
	DrawText(preview.c_str(), static_cast<int>(previewBox.x + 10.0f), static_cast<int>(previewBox.y + 36.0f),
		12, INK);
	DrawText(draft.getType() == CurseVectorType::Poppet
		? "Always available. Weak curse needle; consumes no reagent."
		: "Reagent doses are consumed when this curse is cast.",
		static_cast<int>(previewBox.x + 10.0f), static_cast<int>(previewBox.y + 59.0f), 12, DIM_INK);

	const char* controls = "BACKSPACE undo last   //   C clear doses   //   RELEASE TAB commit";
	const int controlsX = std::max(250, (screenWidth - MeasureText(controls, 14)) / 2);
	DrawText(controls, controlsX,
		static_cast<int>(panel.y + panel.height - 25.0f), 14, INK);
}

const CurseVector& CoatMenu::getDraft() const {
	return draft;
}

bool CoatMenu::wasCommitted() {
	const bool result = committed;
	committed = false;
	return result;
}
