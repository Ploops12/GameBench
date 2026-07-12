#pragma once

#include "Object.hxx"

class Actor : public Object {
public:
	Actor(Vector3 position, float maxHealth, float collisionRadius, float height);

	[[nodiscard]] float health() const;
	[[nodiscard]] float maxHealth() const;
	[[nodiscard]] bool isAlive() const;
	virtual float takeDamage(float damage);
	float heal(float amount);
	void restoreHealth();

	[[nodiscard]] Vector3 velocity() const;
	void setVelocity(Vector3 velocity);
	[[nodiscard]] float height() const;
	[[nodiscard]] BoundingBox bounds() const override;

protected:
	float health_ {100.0f};
	float maxHealth_ {100.0f};
	float height_ {1.8f};
	Vector3 velocity_ {};
};
