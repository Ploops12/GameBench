#include <algorithm>
#include "Actor.hxx"

Actor::Actor(Vector3 position, float radius, float maxHealth)
	: Object(position, radius), health(maxHealth), maxHealth(maxHealth) {
}

void Actor::update(float dt) {
	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
	position.z += velocity.z * dt;
}

const Vector3& Actor::getVelocity() const {
	return velocity;
}

void Actor::setVelocity(const Vector3& value) {
	velocity = value;
}

float Actor::getHealth() const {
	return health;
}

float Actor::getMaxHealth() const {
	return maxHealth;
}

bool Actor::isAlive() const {
	return active && health > 0.0f;
}

bool Actor::takeDamage(float amount) {
	if (!isAlive()) {
		return false;
	}

	health = std::max(0.0f, health - amount);
	if (health <= 0.0f) {
		active = false;
		return true;
	}

	return false;
}

void Actor::heal(float amount) {
	health = std::min(maxHealth, health + amount);
}
