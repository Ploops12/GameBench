#pragma once

#include "Object.hxx"

class Actor : public Object {
public:
	Actor();
	Actor(Vector3 startPosition, float radius, float startingHealth, float startingMoveSpeed);

	void update(float dt) override;

	Vector3 getVelocity() const;
	void setVelocity(Vector3 newVelocity);
	void applyImpulse(Vector3 impulse);

	float getHealth() const;
	float getMaxHealth() const;
	void setHealth(float value);
	void setMaxHealth(float value);
	void takeDamage(float amount);
	bool isAlive() const;

	float getMoveSpeed() const;
	void setMoveSpeed(float value);

protected:
	Vector3 velocity;
	float health;
	float maxHealth;
	float moveSpeed;
};
