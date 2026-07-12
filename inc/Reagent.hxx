#pragma once

#include <raylib.h>

#include <cstddef>

enum class ReagentType : int {
	GraveSalt = 0,
	SaintAsh,
	BlackWax,
	WidowNettle,
	BellBronze,
	MothDust
};

constexpr int REAGENT_COUNT = 6;

struct ReagentDefinition {
	const char* name;
	const char* shortName;
	const char* effect;
	Color color;
	int damage;
	float wardGain;
	float poisonPerSecond;
	float snareDuration;
	float fearDuration;
	float radius;
	float hazardDuration;
	bool seeking;
};

const ReagentDefinition& GetReagentDefinition(ReagentType type);
int ReagentIndex(ReagentType type);
ReagentType ReagentFromIndex(int index);
void DrawReagentIcon(ReagentType type, Vector2 center, float size, Color tint = WHITE);
