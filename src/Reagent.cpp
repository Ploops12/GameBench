#include "Reagent.hxx"

#include <algorithm>
#include <cmath>

namespace {
	constexpr ReagentDefinition REAGENTS[REAGENT_COUNT] = {
		{"Grave Salt", "SALT", "+16 hex damage", {214, 226, 216, 255}, 16, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false},
		{"Saint Ash", "ASH", "+18 ward", {234, 198, 110, 255}, 0, 18.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false},
		{"Black Wax", "WAX", "rot +4 / lingering", {167, 100, 198, 255}, 0, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 2.2f, false},
		{"Widow Nettle", "NETTLE", "thorn snare 1.4s", {114, 214, 128, 255}, 0, 0.0f, 0.0f, 1.4f, 0.0f, 0.0f, 0.0f, false},
		{"Bell Bronze", "BRONZE", "panic 1.8s", {224, 154, 90, 255}, 0, 0.0f, 0.0f, 0.0f, 1.8f, 0.45f, 0.0f, false},
		{"Moth Dust", "MOTH", "+0.8 radius / seeks", {102, 206, 221, 255}, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, true}
	};

	Color iconColor(ReagentType type, Color tint) {
		const Color base = GetReagentDefinition(type).color;
		if (tint.r == 255 && tint.g == 255 && tint.b == 255) return base;
		return {tint.r, tint.g, tint.b, tint.a};
	}

	Vector2 point(Vector2 center, float radius, float angle) {
		return {center.x + std::cos(angle) * radius, center.y + std::sin(angle) * radius};
	}
}

const ReagentDefinition& GetReagentDefinition(ReagentType type) {
	const int index = std::clamp(static_cast<int>(type), 0, REAGENT_COUNT - 1);
	return REAGENTS[index];
}

int ReagentIndex(ReagentType type) {
	return std::clamp(static_cast<int>(type), 0, REAGENT_COUNT - 1);
}

ReagentType ReagentFromIndex(int index) {
	return static_cast<ReagentType>(std::clamp(index, 0, REAGENT_COUNT - 1));
}

void DrawReagentIcon(ReagentType type, Vector2 center, float size, Color tint) {
	const Color color = iconColor(type, tint);
	const float half = size * 0.5f;
	const float line = std::max(1.0f, size * 0.08f);

	DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), half, ColorAlpha(color, 0.35f));
	DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), half * 0.82f, ColorAlpha(color, 0.22f));

	switch (type) {
		case ReagentType::GraveSalt:
			DrawLineEx({center.x - half * 0.42f, center.y}, {center.x + half * 0.42f, center.y}, line, color);
			DrawLineEx({center.x, center.y - half * 0.42f}, {center.x, center.y + half * 0.42f}, line, color);
			DrawLineEx({center.x - half * 0.25f, center.y - half * 0.25f}, {center.x + half * 0.25f, center.y + half * 0.25f}, line, ColorAlpha(color, 0.8f));
			break;
		case ReagentType::SaintAsh:
			DrawTriangle({center.x, center.y - half * 0.43f}, {center.x - half * 0.45f, center.y + half * 0.36f}, {center.x + half * 0.45f, center.y + half * 0.36f}, ColorAlpha(color, 0.28f));
			DrawLineEx({center.x - half * 0.42f, center.y + half * 0.36f}, {center.x + half * 0.42f, center.y + half * 0.36f}, line, color);
			DrawLineEx({center.x, center.y - half * 0.24f}, {center.x, center.y + half * 0.26f}, line, color);
			break;
		case ReagentType::BlackWax:
			DrawCircleV({center.x, center.y + half * 0.12f}, half * 0.28f, ColorAlpha(color, 0.45f));
			DrawLineEx({center.x - half * 0.32f, center.y - half * 0.2f}, {center.x, center.y + half * 0.16f}, line, color);
			DrawLineEx({center.x, center.y + half * 0.16f}, {center.x + half * 0.32f, center.y - half * 0.2f}, line, color);
			DrawLineEx({center.x, center.y - half * 0.28f}, {center.x, center.y - half * 0.05f}, line, color);
			break;
		case ReagentType::WidowNettle:
			DrawLineEx({center.x, center.y + half * 0.4f}, {center.x, center.y - half * 0.4f}, line, color);
			for (int i = -1; i <= 1; ++i) {
				const float y = center.y + static_cast<float>(i) * half * 0.24f;
				DrawLineEx({center.x, y}, {center.x + half * 0.35f, y - half * 0.16f}, line, color);
				DrawLineEx({center.x, y}, {center.x - half * 0.35f, y - half * 0.16f}, line, color);
			}
			break;
		case ReagentType::BellBronze:
			DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), half * 0.35f, color);
			DrawLineEx({center.x - half * 0.36f, center.y + half * 0.25f}, {center.x + half * 0.36f, center.y + half * 0.25f}, line, color);
			DrawLineEx({center.x - half * 0.2f, center.y + half * 0.25f}, {center.x, center.y + half * 0.42f}, line, color);
			DrawLineEx({center.x + half * 0.2f, center.y + half * 0.25f}, {center.x, center.y + half * 0.42f}, line, color);
			break;
		case ReagentType::MothDust:
			DrawLineEx({center.x, center.y - half * 0.32f}, {center.x, center.y + half * 0.36f}, line, color);
			DrawLineEx({center.x - half * 0.05f, center.y - half * 0.08f}, {center.x - half * 0.42f, center.y - half * 0.33f}, line, color);
			DrawLineEx({center.x + half * 0.05f, center.y - half * 0.08f}, {center.x + half * 0.42f, center.y - half * 0.33f}, line, color);
			DrawLineEx({center.x - half * 0.05f, center.y + half * 0.03f}, {center.x - half * 0.38f, center.y + half * 0.32f}, line, color);
			DrawLineEx({center.x + half * 0.05f, center.y + half * 0.03f}, {center.x + half * 0.38f, center.y + half * 0.32f}, line, color);
			break;
	}

	for (int i = 0; i < 4; ++i) {
		const Vector2 spark = point(center, half * 1.12f, static_cast<float>(i) * PI / 2.0f + PI / 4.0f);
		DrawCircleV(spark, line * 0.55f, ColorAlpha(color, 0.7f));
	}
}
