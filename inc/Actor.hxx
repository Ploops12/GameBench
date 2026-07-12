#pragma once

#include "Object.hxx"

class Actor : public Object {
public:
	explicit Actor(Vector3 position = {0.0f, 0.0f, 0.0f});
	~Actor() override = default;

	void update(float deltaTime) override;
	virtual void takeDamage(float amount);

	float health() const;
	float maxHealth() const;
	bool isAlive() const;
	float collisionRadius() const;

protected:
	Vector3 velocity{};
	float currentHealth{100.0f};
	float maximumHealth{100.0f};
	float radius{0.35f};
};
