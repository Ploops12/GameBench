#include "Object.hxx"

Object::Object(Vector3 value) : position(value) {}

void Object::draw3D() const {}

Vector3 Object::getPosition() const {
	return position;
}

void Object::setPosition(Vector3 value) {
	position = value;
}

float Object::getCollisionRadius() const {
	return collisionRadius;
}

void Object::setCollisionRadius(float value) {
	collisionRadius = value;
}

bool Object::isActive() const {
	return active;
}

void Object::setActive(bool value) {
	active = value;
}
