#include "Actor.hxx"

#include <algorithm>

Actor::Actor(Vector3 position, float initialHealth) : Object(position), health(initialHealth), maxHealth(initialHealth) {}

float Actor::getHealth() const {
	return health;
}

float Actor::getMaxHealth() const {
	return maxHealth;
}

bool Actor::isDead() const {
	return health <= 0.0f;
}

void Actor::takeDamage(float amount) {
	health = std::max(0.0f, health - amount);
}
