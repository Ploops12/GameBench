#include "CoatMenu.h"
#include "HUD.h"
#include "Player.hxx"

#include <raylib.h>
#include <algorithm>
#include <cstdio>

namespace {
void Panel(Rectangle r, Color color) {
	DrawRectangleRec(r, Fade(BLACK, 0.82f));
	DrawRectangleLinesEx(r, 2.0f, color);
}

void Meter(int x, int y, int width, float value, float maximum, Color color) {
	DrawRectangleLines(x, y, width, 12, Fade(RAYWHITE, 0.7f));
	DrawRectangle(x + 2, y + 2, static_cast<int>((width - 4) * std::clamp(value / maximum, 0.0f, 1.0f)), 8, color);
}
}

void CoatMenu::clear() {
	spell = {};
	cursor = 0;
}

void CoatMenu::update(Player& player) {
	if (IsKeyPressed(KEY_ONE)) spell.type = VectorType::Poppet;
	if (IsKeyPressed(KEY_TWO)) spell.type = VectorType::Bottle;
	if (IsKeyPressed(KEY_THREE)) spell.type = VectorType::Apple;
	if (IsKeyPressed(KEY_FOUR)) spell.type = VectorType::Spider;
	const int reagentKeys[ReagentCount] = {KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N};
	for (int i = 0; i < ReagentCount; ++i) {
		if (IsKeyPressed(reagentKeys[i]) && spell.reagentSlots < 3 && player.inventory[i] > 0) {
			spell.reagents[spell.reagentSlots++] = static_cast<ReagentType>(i);
			player.inventory[i]--;
		}
	}
	if (IsKeyPressed(KEY_BACKSPACE) && spell.reagentSlots > 0) {
		const int index = static_cast<int>(spell.reagents[--spell.reagentSlots]);
		player.inventory[index]++;
	}
}

void CoatMenu::draw(const Player& player) const {
	const int w = GetScreenWidth();
	const int h = GetScreenHeight();
	Rectangle panel{70.0f, h * 0.52f, static_cast<float>(w - 140), h * 0.40f};
	Panel(panel, Color{229, 205, 137, 255});
	DrawText("COAT OPEN // YOU ARE SLOW // SPRINT + JUMP DISABLED", 92, static_cast<int>(panel.y + 14), 18, Color{235, 86, 68, 255});
	DrawText("BASE VECTOR", 92, static_cast<int>(panel.y + 46), 16, RAYWHITE);
	const char* bases[] = {"[1] POPPET", "[2] BOTTLE", "[3] APPLE", "[4] SPIDER"};
	for (int i = 0; i < 4; ++i) {
		Color c = static_cast<int>(spell.type) == i ? Color{229, 205, 137, 255} : GRAY;
		DrawText(bases[i], 92 + i * 150, static_cast<int>(panel.y + 68), 18, c);
	}
	DrawText("REAGENTS // Z X C V B N // repeat to strengthen // BACKSPACE undo", 92, static_cast<int>(panel.y + 103), 16, RAYWHITE);
	const char keys[] = {'Z', 'X', 'C', 'V', 'B', 'N'};
	for (int i = 0; i < ReagentCount; ++i) {
		const ReagentInfo& info = GetReagentInfo(static_cast<ReagentType>(i));
		const int x = 92 + (i % 3) * 215;
		const int y = static_cast<int>(panel.y + 128 + (i / 3) * 48);
		DrawCircleLines(x + 10, y + 10, 9.0f, info.color);
		char label[80];
		std::snprintf(label, sizeof(label), "[%c] %s x%d", keys[i], info.name, player.inventory[i]);
		DrawText(label, x + 25, y, 16, player.inventory[i] > 0 ? info.color : DARKGRAY);
		DrawText(info.effect, x + 25, y + 19, 12, GRAY);
	}
	DrawText(spell.name(), 92, static_cast<int>(panel.y + panel.height - 72), 21, Color{229, 205, 137, 255});
	DrawText(spell.ingredients().c_str(), 92, static_cast<int>(panel.y + panel.height - 48), 14, LIGHTGRAY);
	DrawText(spell.preview().c_str(), 92, static_cast<int>(panel.y + panel.height - 27), 14, RAYWHITE);
}

void HUD::draw(const Player& player, const CoatMenu& coat, int enemies, float messageTime, const char* message) const {
	const int w = GetScreenWidth();
	const int h = GetScreenHeight();
	Panel({18, 18, 220, 78}, Color{229, 205, 137, 255});
	DrawText("FLESH", 30, 29, 15, RAYWHITE);
	Meter(92, 31, 128, player.health, player.maxHealth, Color{211, 63, 55, 255});
	DrawText("WARD [Q]", 30, 56, 15, player.wardActive ? SKYBLUE : RAYWHITE);
	Meter(110, 58, 110, player.ward, player.maxWard, Color{91, 188, 207, 255});
	char threat[64];
	std::snprintf(threat, sizeof(threat), "HUNTERS REMAIN: %d", enemies);
	DrawText(threat, w / 2 - MeasureText(threat, 17) / 2, 24, 17, Color{229, 205, 137, 255});
	Panel({static_cast<float>(w - 275), 18, 257, 78}, Color{229, 205, 137, 255});
	DrawText("HOLD [R] RUMMAGE", w - 258, 30, 17, coat.open ? Color{235, 86, 68, 255} : RAYWHITE);
	DrawText("LMB CAST  |  SHIFT SPRINT", w - 258, 56, 14, GRAY);
	DrawText(coat.spell.name(), 22, h - 91, 20, Color{229, 205, 137, 255});
	DrawText(coat.spell.ingredients().c_str(), 22, h - 67, 13, Color{229, 205, 137, 255});
	DrawText(coat.spell.preview().c_str(), 22, h - 46, 13, LIGHTGRAY);
	DrawText("WASD MOVE  SPACE JUMP  Q WARD  R COAT  ESC MOUSE", 22, h - 23, 13, GRAY);
	DrawLine(w / 2 - 8, h / 2, w / 2 + 8, h / 2, RAYWHITE);
	DrawLine(w / 2, h / 2 - 8, w / 2, h / 2 + 8, RAYWHITE);
	if (messageTime > 0.0f) {
		const int tw = MeasureText(message, 20);
		Panel({static_cast<float>(w / 2 - tw / 2 - 16), 106, static_cast<float>(tw + 32), 42}, Color{229, 205, 137, 255});
		DrawText(message, w / 2 - tw / 2, 117, 20, RAYWHITE);
	}
}
