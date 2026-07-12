#include "Object.hxx"

Object::Object(Vector3 initialPosition, float initialCollisionRadius)
	: position(initialPosition), collisionRadius(initialCollisionRadius) {
}

void Object::update(float) {
}

void Object::draw3D() const {
}

const Vector3& Object::getPosition() const {
	return position;
}

void Object::setPosition(Vector3 newPosition) {
	position = newPosition;
}

float Object::getCollisionRadius() const {
	return collisionRadius;
}

bool Object::isCollisionEnabled() const {
	return collisionEnabled;
}

void Object::setCollisionEnabled(bool enabled) {
	collisionEnabled = enabled;
}

bool Object::isVisible() const {
	return visible;
}

void Object::setVisible(bool shouldDraw) {
	visible = shouldDraw;
}
