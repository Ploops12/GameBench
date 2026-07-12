#pragma once

#include "Object.hxx"

class Actor : public Object {
	// Movable, damageable world object with simple curse status support.
public:
	Actor(Vector3 position = {}, Vector3 size = {0.8f, 1.8f, 0.8f}, float maxHealth = 100.0f);
	~Actor() override = default;

	void update(float deltaTime) override;
	virtual float takeDamage(float damage);
	void heal(float amount);

	float getHealth() const;
	float getMaxHealth() const;
	float getHealthRatio() const;
	bool isAlive() const;

	Vector3 getVelocity() const;
	void setVelocity(Vector3 value);

	void applySnare(float duration, float speedMultiplier = 0.45f);
	void applyRot(float damagePerSecond, float duration);
	void applyFear(float duration);
	float getMovementMultiplier() const;
	float getFearTime() const;
	float getHurtTime() const;

protected:
	Vector3 velocity {};
	float health {100.0f};
	float maxHealth {100.0f};
	float hurtTime {0.0f};
	float snareTime {0.0f};
	float snareMultiplier {1.0f};
	float rotTime {0.0f};
	float rotDamagePerSecond {0.0f};
	float fearTime {0.0f};
};
