#pragma once

#include <raylib.h>

enum class EffectType {
	Impact,
	Hazard,
	WardPulse
};

class Effect {
public:
	Effect(EffectType type, Vector3 position, float radius, float lifetime);
	void Update(float dt);
	void Draw() const;
	bool IsAlive() const { return lifetime > 0.0f; }
	EffectType GetType() const { return type; }
	Vector3 GetPosition() const { return position; }
	float GetRadius() const { return radius; }

private:
	EffectType type;
	Vector3 position{};
	float radius{1.0f};
	float lifetime{0.0f};
	float totalLifetime{0.0f};
};
