#include "Object.hxx"
#include "Actor.hxx"
#include <algorithm>

Object::Object(Vector3 position, float collisionRadius) :
	position(position), collisionRadius(collisionRadius) {}

void Object::update(float) {}

Actor::Actor(Vector3 position, float radius, float maxHealth) :
	Object(position, radius), health(maxHealth), maxHealth(maxHealth) {}

void Actor::damage(float amount) {
	health = std::max(0.0f, health - amount);
	if (health <= 0.0f) active = false;
}

void Actor::heal(float amount) {
	health = std::min(maxHealth, health + amount);
}
