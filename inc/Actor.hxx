#pragma once

#include "Object.hxx"

class Actor : public Object {
public:
	Actor(Vector3 position = {}, float radius = 0.45f, float maxHealth = 100.0f);

	void update(float dt) override;

	const Vector3& getVelocity() const;
	void setVelocity(const Vector3& value);

	float getHealth() const;
	float getMaxHealth() const;
	bool isAlive() const;

	bool takeDamage(float amount);
	void heal(float amount);
protected:
	Vector3 velocity {};
	float health {100.0f};
	float maxHealth {100.0f};
};
