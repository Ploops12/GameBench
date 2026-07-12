#include "CoatMenu.h"

namespace {
	const char* VectorName(VectorType vector) {
		switch (vector) {
			case VectorType::Bottle: return "BOTTLE";
			case VectorType::Apple: return "APPLE";
			case VectorType::Spider: return "SPIDER";
		}
		return "?";
	}

	int SelectedCount(const CurseVector& curse, ReagentType type) {
		int count = 0;
		for (int i = 0; i < curse.GetReagentCount(); ++i) {
			if (curse.GetReagents()[i] == type) ++count;
		}
		return count;
	}
}

void CoatMenu::Toggle() {
	open = !open;
}

void CoatMenu::Update(CurseVector& curse, const std::array<int, REAGENT_COUNT>& inventory) {
	if (!open) return;
	const Vector2 mouse = GetMousePosition();
	const int width = GetScreenWidth();
	const int panelX = 28;
	const int panelY = 42;
	const float baseX = width * 0.5f - 300.0f;
	const float baseY = panelY + 78.0f;
	const Rectangle basePanel{baseX, baseY, 600.0f, 82.0f};
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, basePanel)) {
		const int slot = static_cast<int>((mouse.x - basePanel.x) / 200.0f);
		if (slot >= 0 && slot < 3) curse.SetVector(static_cast<VectorType>(slot));
		return;
	}
	const float reagentX = panelX + 24.0f;
	const float reagentY = baseY + 132.0f;
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		for (int i = 0; i < REAGENT_COUNT; ++i) {
			const int column = i % 3;
			const int row = i / 3;
			const Rectangle slot{reagentX + column * 215.0f, reagentY + row * 86.0f, 202.0f, 70.0f};
			if (CheckCollisionPointRec(mouse, slot)) {
				curse.AddReagent(static_cast<ReagentType>(i), inventory[i]);
				return;
			}
		}
	}
}

void CoatMenu::SelectVector(CurseVector& curse, VectorType vector) const {
	if (open) curse.SetVector(vector);
}

bool CoatMenu::AddReagent(CurseVector& curse, ReagentType type, const std::array<int, REAGENT_COUNT>& inventory) const {
	return open && curse.AddReagent(type, inventory[static_cast<int>(type)]);
}

void CoatMenu::Draw(const std::array<int, REAGENT_COUNT>& inventory, const CurseVector& curse) const {
	if (!open) return;
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	const int panelX = 28;
	const int panelY = 42;
	const int panelW = width - 56;
	const int panelH = height - 84;
	DrawRectangle(0, 0, width, height, Fade(Color{4, 8, 12, 255}, 0.74f));
	DrawRectangle(panelX, panelY, panelW, panelH, Fade(Color{10, 17, 22, 255}, 0.96f));
	DrawRectangleLinesEx({static_cast<float>(panelX), static_cast<float>(panelY), static_cast<float>(panelW), static_cast<float>(panelH)}, 2.0f, BEIGE);
	DrawRectangleLinesEx({panelX + 8.0f, panelY + 8.0f, panelW - 16.0f, panelH - 16.0f}, 1.0f, Fade(ORANGE, 0.8f));
	DrawText("COAT RUMMAGE // WORLD LIVE", panelX + 24, panelY + 20, 22, ORANGE);
	DrawText("R CLOSE    TAB: HOLD RUMMAGE    1/2/3: VECTOR    Q E G H J K: ADD REAGENT", panelX + 24, panelY + 48, 14, LIGHTGRAY);
	DrawText("MOVEMENT SLOWED 55%    SPRINT DISABLED    VULNERABLE WHILE SEARCHING", panelX + 24, panelY + panelH - 28, 15, Color{255, 126, 110, 255});

	const float baseX = width * 0.5f - 300.0f;
	const float baseY = panelY + 78.0f;
	for (int i = 0; i < 3; ++i) {
		const Rectangle slot{baseX + i * 200.0f, baseY, 184.0f, 82.0f};
		const VectorType vector = static_cast<VectorType>(i);
		const bool active = curse.GetVector() == vector;
		DrawRectangleRec(slot, Fade(active ? ORANGE : Color{28, 38, 45, 255}, active ? 0.28f : 0.92f));
		DrawRectangleLinesEx(slot, 2.0f, active ? ORANGE : DARKGRAY);
		DrawText(TextFormat("[%d] %s", i + 1, VectorName(vector)), static_cast<int>(slot.x + 14), static_cast<int>(slot.y + 12), 18, active ? ORANGE : BEIGE);
		DrawText(i == 0 ? "thrown curse" : (i == 1 ? "eat / self-target" : "place trap"), static_cast<int>(slot.x + 14), static_cast<int>(slot.y + 43), 13, LIGHTGRAY);
	}

	DrawText("SELECT UP TO THREE REAGENT STROKES", panelX + 24, baseY + 110, 17, BEIGE);
	const float reagentX = panelX + 24.0f;
	const float reagentY = baseY + 132.0f;
	for (int i = 0; i < REAGENT_COUNT; ++i) {
		const int column = i % 3;
		const int row = i / 3;
		const Rectangle slot{reagentX + column * 215.0f, reagentY + row * 86.0f, 202.0f, 70.0f};
		const ReagentType type = static_cast<ReagentType>(i);
		const Color color = ReagentColor(type);
		const int selected = SelectedCount(curse, type);
		DrawRectangleRec(slot, Fade(Color{18, 26, 32, 255}, 0.96f));
		DrawRectangleLinesEx(slot, 1.0f, Fade(color, 0.8f));
		DrawCircle(static_cast<int>(slot.x + 25), static_cast<int>(slot.y + 35), 13.0f, Fade(color, 0.85f));
		DrawCircleLines(static_cast<int>(slot.x + 25), static_cast<int>(slot.y + 35), 17.0f, color);
		DrawText(TextFormat("[%s] %s", ReagentKey(type), ReagentName(type)), static_cast<int>(slot.x + 52), static_cast<int>(slot.y + 13), 14, color);
		DrawText(TextFormat("STOCK %d   IN HEX %d", inventory[i], selected), static_cast<int>(slot.x + 52), static_cast<int>(slot.y + 39), 12, selected > 0 ? ORANGE : LIGHTGRAY);
	}

	const SpellStats stats = curse.CalculateStats();
	const int previewY = height - 190;
	DrawRectangle(panelX + 24, previewY, panelW - 48, 105, Fade(Color{20, 30, 35, 255}, 0.98f));
	DrawRectangleLinesEx({panelX + 24.0f, static_cast<float>(previewY), static_cast<float>(panelW - 48), 105.0f}, 2.0f, ORANGE);
	DrawText("SPELL PREVIEW", panelX + 42, previewY + 13, 16, ORANGE);
	DrawText(curse.GetName().c_str(), panelX + 42, previewY + 40, 18, WHITE);
	DrawText(TextFormat("DAMAGE %.0f   SPLASH %.1f   SNARE %.1fs   POISON %.0f   WARD +%.0f", stats.damage, stats.splashRadius, stats.snareSeconds, stats.poisonDamage, stats.wardGain), panelX + 42, previewY + 68, 14, LIGHTGRAY);
	DrawText(TextFormat("FEAR %.1fs   HAZARD %.1fs   %s", stats.fearSeconds, stats.hazardSeconds, stats.seeks ? "SEEKING" : "STRAIGHT FLIGHT"), panelX + 42, previewY + 86, 14, stats.seeks ? SKYBLUE : LIGHTGRAY);
}
