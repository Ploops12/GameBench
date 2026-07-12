#pragma once

#include "CurseVector.hxx"
#include <raylib.h>

enum class EffectKind {
	Projectile,
	EnemyShot,
	Mine,
	Hazard,
	Burst
};

struct Effect {
	EffectKind kind{EffectKind::Projectile};
	SpellDraft draft{};
	SpellStats stats{};
	Vector3 position{};
	Vector3 velocity{};
	float age{0.0f};
	float lifetime{2.5f};
	float radius{0.4f};
	bool armed{true};
	bool active{true};
	bool hostile{false};
};
