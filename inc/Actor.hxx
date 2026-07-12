#pragma once

#include "Object.hxx"

class Actor : public Object {
public:
	Actor(Vector3 position = {}, float collisionRadius = 0.45f, float maxHealth = 100.0f);

	virtual void takeDamage(float amount);
	void heal(float amount);
	bool isAlive() const;
	float getHealth() const;
	float getMaxHealth() const;
	float getHealthRatio() const;
	const Vector3& getVelocity() const;
	void setVelocity(Vector3 newVelocity);

protected:
	float health {100.0f};
	float maxHealth {100.0f};
	Vector3 velocity {};
};
