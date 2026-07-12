#include "Object.hxx"

Object::Object() : position {0.0f, 0.0f, 0.0f}, collisionRadius(0.5f), active(true), collidable(false) {}

Object::Object(Vector3 startPosition, float radius, bool isCollidable)
	: position(startPosition), collisionRadius(radius), active(true), collidable(isCollidable) {}

void Object::update(float) {}

void Object::draw3D(const Camera3D&) const {}

void Object::draw2D() const {}

Vector3 Object::getPosition() const {
	return position;
}

void Object::setPosition(Vector3 newPosition) {
	position = newPosition;
}

float Object::getCollisionRadius() const {
	return collisionRadius;
}

void Object::setCollisionRadius(float radius) {
	collisionRadius = radius;
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

BoundingBox Object::getBoundingBox() const {
	return {
		{position.x - collisionRadius, position.y - collisionRadius, position.z - collisionRadius},
		{position.x + collisionRadius, position.y + collisionRadius, position.z + collisionRadius}
	};
}
