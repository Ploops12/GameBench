#include "Actor.hxx"

#include <algorithm>

namespace {

float tickTimer(float value, float deltaTime) {
	return std::max(0.0f, value - deltaTime);
}

} // namespace

Actor::Actor(Vector3 initialPosition, Vector3 initialSize, float initialMaxHealth)
	: Object(initialPosition, initialSize),
	  health(std::max(0.0f, initialMaxHealth)),
	  maxHealth(std::max(0.0f, initialMaxHealth)) {
}

void Actor::update(float deltaTime) {
	if (!active || deltaTime <= 0.0f) return;

	Object::update(deltaTime);
	hurtTime = tickTimer(hurtTime, deltaTime);
	fearTime = tickTimer(fearTime, deltaTime);

	if (snareTime > 0.0f) {
		snareTime = tickTimer(snareTime, deltaTime);
		if (snareTime <= 0.0f) snareMultiplier = 1.0f;
	}

	if (rotTime > 0.0f) {
		const float damageTime = std::min(deltaTime, rotTime);
		rotTime = tickTimer(rotTime, deltaTime);
		if (rotDamagePerSecond > 0.0f && isAlive()) {
			takeDamage(rotDamagePerSecond * damageTime);
		}
		if (rotTime <= 0.0f) rotDamagePerSecond = 0.0f;
	}
}

float Actor::takeDamage(float damage) {
	if (damage <= 0.0f || !isAlive()) return 0.0f;

	const float appliedDamage = std::min(damage, health);
	health -= appliedDamage;
	if (health <= 0.0f) health = 0.0f;
	hurtTime = 0.18f;
	return appliedDamage;
}

void Actor::heal(float amount) {
	if (amount <= 0.0f || maxHealth <= 0.0f) return;
	health = std::min(maxHealth, health + amount);
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

bool Actor::isAlive() const {
	return health > 0.0f;
}

Vector3 Actor::getVelocity() const {
	return velocity;
}

void Actor::setVelocity(Vector3 value) {
	velocity = value;
}

void Actor::applySnare(float duration, float speedMultiplier) {
	if (duration <= 0.0f) return;

	snareTime = std::max(snareTime, duration);
	snareMultiplier = std::min(snareMultiplier, std::clamp(speedMultiplier, 0.05f, 1.0f));
}

void Actor::applyRot(float damagePerSecond, float duration) {
	if (damagePerSecond <= 0.0f || duration <= 0.0f) return;

	if (rotTime <= 0.0f || damagePerSecond >= rotDamagePerSecond) {
		rotTime = duration;
		rotDamagePerSecond = damagePerSecond;
	}
}

void Actor::applyFear(float duration) {
	if (duration <= 0.0f) return;
	fearTime = std::max(fearTime, duration);
}

float Actor::getMovementMultiplier() const {
	return snareTime > 0.0f ? snareMultiplier : 1.0f;
}

float Actor::getFearTime() const {
	return fearTime;
}

float Actor::getHurtTime() const {
	return hurtTime;
}
