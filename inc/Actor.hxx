#pragma once

// Moving/damageable object base for player and enemies.

#include "Object.hxx"

class Actor : public Object {
public:
	Actor() = default;
	Actor(Vector3 startPosition, float collisionRadius, float maxHealth);

	void update(float dt) override;
	void applyDamage(float amount);
	void heal(float amount);
	bool isAlive() const;

	Vector3 velocity{};
	float health{100.0f};
	float maxHealth{100.0f};
};
