#pragma once

#include "Object.hxx"

class Actor : public Object {
public:
	Actor(Vector3 position = {}, float collisionRadius = 0.4f, float maximumHealth = 100.0f);
	~Actor() override = default;

	float getHealth() const;
	float getMaximumHealth() const;
	bool isAlive() const;
	virtual void takeDamage(float amount);
	void heal(float amount);

protected:
	float health {100.0f};
	float maximumHealth {100.0f};
};
