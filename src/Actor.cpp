#include "Actor.hxx"

#include <algorithm>

Actor::Actor(Vector3 initialPosition, float initialCollisionRadius, float initialMaximumHealth)
	: Object(initialPosition, initialCollisionRadius), health(initialMaximumHealth), maximumHealth(initialMaximumHealth) {
}

float Actor::getHealth() const {
	return health;
}

float Actor::getMaximumHealth() const {
	return maximumHealth;
}

bool Actor::isAlive() const {
	return health > 0.0f;
}

void Actor::takeDamage(float amount) {
	if (amount <= 0.0f || !isAlive()) {
		return;
	}

	health = std::max(0.0f, health - amount);
	if (health <= 0.0f) {
		active = false;
	}
}

void Actor::heal(float amount) {
	if (amount <= 0.0f || !isAlive()) {
		return;
	}

	health = std::min(maximumHealth, health + amount);
}
