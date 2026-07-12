#include "Object.hxx"

Object::Object(Vector3 initialPosition, float initialCollisionRadius)
	: position(initialPosition), collisionRadius(initialCollisionRadius) {
}

void Object::update(float) {
}

void Object::draw(const Camera3D&) const {
}

const Vector3& Object::getPosition() const {
	return position;
}

void Object::setPosition(Vector3 value) {
	position = value;
}

float Object::getCollisionRadius() const {
	return collisionRadius;
}

bool Object::isActive() const {
	return active;
}

void Object::setActive(bool value) {
	active = value;
}

bool Object::isDrawable() const {
	return drawable;
}

void Object::setDrawable(bool value) {
	drawable = value;
}

bool Object::hasCollision() const {
	return collisionEnabled;
}

void Object::setCollisionEnabled(bool value) {
	collisionEnabled = value;
}

bool Object::overlapsSphere(Vector3 center, float radius) const {
	return collisionEnabled && CheckCollisionSpheres(position, collisionRadius, center, radius);
}
