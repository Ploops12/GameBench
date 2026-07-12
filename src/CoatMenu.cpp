#include "CoatMenu.h"
#include <algorithm>

namespace {
bool CanAddReagent(const Player& player, int reagentIndex) {
	int selected = 0;
	for (int i = 0; i < player.draft.reagentCount; ++i) {
		if (static_cast<int>(player.draft.reagents[i]) == reagentIndex) selected++;
	}

	return player.draft.reagentCount < 3 && selected < player.reagents[reagentIndex];
}
}

void CoatMenu::update(Player& player) {
	if (!open) return;

	if (IsKeyPressed(KEY_Q)) {
		int base = static_cast<int>(player.draft.base);
		base = (base + 1) % static_cast<int>(BaseVector::Count);
		player.draft.base = static_cast<BaseVector>(base);
	}

	if (IsKeyPressed(KEY_C)) {
		player.draft.reagentCount = 0;
	}

	if ((IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsKeyPressed(KEY_BACKSPACE)) && player.draft.reagentCount > 0) {
		player.draft.reagentCount--;
	}

	float wheel = GetMouseWheelMove();
	if (wheel != 0.0f) {
		highlightedReagent -= static_cast<int>(wheel);
		if (highlightedReagent < 0) highlightedReagent = REAGENT_COUNT - 1;
		if (highlightedReagent >= REAGENT_COUNT) highlightedReagent = 0;
	}

	for (int i = 0; i < REAGENT_COUNT; ++i) {
		if (IsKeyPressed(KEY_ONE + i)) {
			highlightedReagent = i;
			if (CanAddReagent(player, i)) {
				player.draft.reagents[player.draft.reagentCount++] = static_cast<ReagentType>(i);
			}
		}
	}

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CanAddReagent(player, highlightedReagent)) {
		player.draft.reagents[player.draft.reagentCount++] = static_cast<ReagentType>(highlightedReagent);
	}
}

void CoatMenu::draw(const Player& player, int screenWidth, int screenHeight) const {
	if (!open) return;

	const int panelX = 34;
	const int panelY = screenHeight - 260;
	const int panelW = screenWidth - 68;
	const int panelH = 220;
	const Color ink = Color{230, 245, 220, 255};
	const Color dim = Color{95, 120, 100, 255};
	const Color panel = Color{8, 15, 12, 220};

	DrawRectangle(panelX, panelY, panelW, panelH, panel);
	DrawRectangleLinesEx(Rectangle{static_cast<float>(panelX), static_cast<float>(panelY), static_cast<float>(panelW), static_cast<float>(panelH)}, 2.0f, ink);
	DrawText("COAT RUMMAGE - MOVING SLOW / SPRINT LOCKED", panelX + 18, panelY + 12, 18, ink);
	DrawText(TextFormat("Q BASE: %s", GetBaseVectorDefinition(player.draft.base).name.data()), panelX + 18, panelY + 42, 18, Color{255, 210, 110, 255});
	DrawText("1-6 ADD   WHEEL AIM POCKET   LMB ADD   RMB/BACKSPACE REMOVE   C CLEAR", panelX + 18, panelY + 68, 14, dim);

	for (int i = 0; i < REAGENT_COUNT; ++i) {
		const ReagentDefinition& reagent = GetReagentDefinitions()[i];
		int x = panelX + 20 + i * ((panelW - 40) / REAGENT_COUNT);
		int y = panelY + 102;
		int w = (panelW - 62) / REAGENT_COUNT;
		Color border = i == highlightedReagent ? Color{255, 250, 160, 255} : dim;
		DrawRectangleLinesEx(Rectangle{static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), 54.0f}, 2.0f, border);
		DrawCircleLines(x + 18, y + 25, 11.0f, reagent.color);
		DrawText(TextFormat("%d %s", i + 1, reagent.shortName.data()), x + 36, y + 9, 13, ink);
		DrawText(TextFormat("x%d", player.reagents[i]), x + 36, y + 29, 13, player.reagents[i] > 0 ? ink : Color{150, 70, 70, 255});
	}

	int draftY = panelY + 170;
	DrawText("SELECTED:", panelX + 18, draftY, 15, ink);
	for (int i = 0; i < 3; ++i) {
		int x = panelX + 112 + i * 82;
		DrawRectangleLines(x, draftY - 5, 72, 28, i < player.draft.reagentCount ? ink : dim);
		if (i < player.draft.reagentCount) {
			const ReagentDefinition& reagent = GetReagentDefinition(player.draft.reagents[i]);
			DrawText(reagent.shortName.data(), x + 7, draftY + 2, 12, reagent.color);
		}
	}

	std::string preview = BuildSpellPreview(player.draft);
	DrawText(preview.c_str(), panelX + 370, draftY, 15, player.hasDraftReagents() ? Color{210, 255, 190, 255} : Color{255, 105, 95, 255});
	if (!player.hasDraftReagents()) {
		DrawText("MISSING REAGENTS", panelX + panelW - 176, panelY + 42, 16, Color{255, 105, 95, 255});
	}
}
