#pragma once

#include "Object.hxx"

class Actor : public Object {
public:
	explicit Actor(Vector3 position = {}, float health = 1.0f);

	float getHealth() const;
	float getMaxHealth() const;
	bool isDead() const;
	virtual void takeDamage(float amount);

protected:
	Vector3 velocity {};
	float health {1.0f};
	float maxHealth {1.0f};
};
