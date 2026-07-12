#include "Actor.hxx"

#include <algorithm>

Actor::Actor(Vector3 initialPosition, float initialCollisionRadius, float initialMaxHealth)
	: Object(initialPosition, initialCollisionRadius), health(initialMaxHealth), maxHealth(initialMaxHealth) {
}

void Actor::takeDamage(float amount) {
	health = std::max(0.0f, health - std::max(0.0f, amount));
}

void Actor::heal(float amount) {
	health = std::min(maxHealth, health + std::max(0.0f, amount));
}

bool Actor::isAlive() const {
	return health > 0.0f;
}

float Actor::getHealth() const {
	return health;
}

float Actor::getMaxHealth() const {
	return maxHealth;
}

float Actor::getHealthRatio() const {
	return maxHealth > 0.0f ? health / maxHealth : 0.0f;
}

const Vector3& Actor::getVelocity() const {
	return velocity;
}

void Actor::setVelocity(Vector3 newVelocity) {
	velocity = newVelocity;
}
