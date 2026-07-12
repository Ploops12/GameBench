#include <algorithm>
#include "Actor.hxx"
#include <raymath.h>

Actor::Actor() : Object(), velocity {0.0f, 0.0f, 0.0f}, health(100.0f), maxHealth(100.0f), moveSpeed(4.0f) {}

Actor::Actor(Vector3 startPosition, float radius, float startingHealth, float startingMoveSpeed)
	: Object(startPosition, radius, true),
		velocity {0.0f, 0.0f, 0.0f},
		health(startingHealth),
		maxHealth(startingHealth),
		moveSpeed(startingMoveSpeed) {}

void Actor::update(float dt) {
	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
	position.z += velocity.z * dt;
}

Vector3 Actor::getVelocity() const {
	return velocity;
}

void Actor::setVelocity(Vector3 newVelocity) {
	velocity = newVelocity;
}

void Actor::applyImpulse(Vector3 impulse) {
	velocity = Vector3Add(velocity, impulse);
}

float Actor::getHealth() const {
	return health;
}

float Actor::getMaxHealth() const {
	return maxHealth;
}

void Actor::setHealth(float value) {
	health = std::clamp(value, 0.0f, maxHealth);
}

void Actor::setMaxHealth(float value) {
	maxHealth = value;
	health = std::min(health, maxHealth);
}

void Actor::takeDamage(float amount) {
	health = std::max(0.0f, health - amount);
	if (health <= 0.0f) {
		active = false;
	}
}

bool Actor::isAlive() const {
	return active && health > 0.0f;
}

float Actor::getMoveSpeed() const {
	return moveSpeed;
}

void Actor::setMoveSpeed(float value) {
	moveSpeed = value;
}
