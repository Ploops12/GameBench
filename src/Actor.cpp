#include "Actor.hxx"

#include <algorithm>

Actor::Actor(Vector3 position, float maxHealth, float collisionRadius, float height)
	: Object(position, collisionRadius), health_(maxHealth), maxHealth_(maxHealth), height_(height) {
}

float Actor::health() const {
	return health_;
}

float Actor::maxHealth() const {
	return maxHealth_;
}

bool Actor::isAlive() const {
	return health_ > 0.0f;
}

float Actor::takeDamage(float damage) {
	if (!isAlive() || damage <= 0.0f) {
		return 0.0f;
	}
	const float before = health_;
	health_ = std::max(0.0f, health_ - damage);
	if (health_ <= 0.0f) {
		active_ = false;
	}
	return before - health_;
}

float Actor::heal(float amount) {
	if (!isAlive() || amount <= 0.0f) {
		return 0.0f;
	}
	const float before = health_;
	health_ = std::min(maxHealth_, health_ + amount);
	return health_ - before;
}

void Actor::restoreHealth() {
	health_ = maxHealth_;
	active_ = true;
}

Vector3 Actor::velocity() const {
	return velocity_;
}

void Actor::setVelocity(Vector3 velocity) {
	velocity_ = velocity;
}

float Actor::height() const {
	return height_;
}

BoundingBox Actor::bounds() const {
	return {
		{position_.x - collisionRadius_, position_.y, position_.z - collisionRadius_},
		{position_.x + collisionRadius_, position_.y + height_, position_.z + collisionRadius_}
	};
}
