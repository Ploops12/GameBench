#pragma once

#include <raylib.h>
#include <raymath.h>
#include "CurseVector.hxx"

enum class EffectKind { Bolt, Mine, Burst };

struct CurseEffect {
	EffectKind kind{EffectKind::Bolt};
	Vector3 position{};
	Vector3 velocity{};
	SpellStats stats{};
	float life{2.0f};
	bool active{true};

	void update(float dt) { position = Vector3Add(position, Vector3Scale(velocity, dt)); life -= dt; active = active && life > 0.0f; }
	void draw() const;
};
