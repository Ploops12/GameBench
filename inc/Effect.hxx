#pragma once

// Runtime in-world spell effects: projectiles, lingering hazards, spider mines, and impact flashes.

#include "CurseVector.hxx"

#include <raylib.h>

struct Projectile {
	Vector3 position{};
	Vector3 velocity{};
	CurseSpell spell{};
	float age{};
	float maxAge{4.0f};
	bool active{true};
};

struct LingeringHazard {
	Vector3 position{};
	SpellStats stats{};
	float age{};
	float pulseTimer{};
	bool active{true};
};

struct SpiderMine {
	Vector3 position{};
	CurseSpell spell{};
	float age{};
	bool active{true};
	bool armed{};
};

struct ImpactFlash {
	Vector3 position{};
	Color color{};
	float age{};
	float lifetime{0.4f};
	bool active{true};
};
