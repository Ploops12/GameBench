#include "Object.hxx"

Object::Object(Vector3 position, float collisionRadius)
	: position(position), collisionRadius(collisionRadius) {
}

void Object::update(float) {
}

bool Object::isActive() const {
	return active;
}

void Object::setActive(bool value) {
	active = value;
}

bool Object::isCollidable() const {
	return collidable;
}

void Object::setCollidable(bool value) {
	collidable = value;
}

float Object::getCollisionRadius() const {
	return collisionRadius;
}

const Vector3& Object::getPosition() const {
	return position;
}

void Object::setPosition(const Vector3& value) {
	position = value;
}
