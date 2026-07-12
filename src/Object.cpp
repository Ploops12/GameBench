#include "Object.hxx"

#include <cmath>

namespace {

Vector3 normaliseSize(Vector3 value) {
	return {
		std::fabs(value.x),
		std::fabs(value.y),
		std::fabs(value.z)
	};
}

} // namespace

Object::Object(Vector3 initialPosition, Vector3 initialSize)
	: position(initialPosition), size(normaliseSize(initialSize)) {
}

void Object::update(float deltaTime) {
	(void)deltaTime;
}

void Object::draw3D(const Camera3D& camera) const {
	(void)camera;
	if (!visible) return;

	DrawCubeV(position, size, Fade(DARKGRAY, 0.2f));
	DrawCubeWiresV(position, size, GRAY);
}

Vector3 Object::getPosition() const {
	return position;
}

void Object::setPosition(Vector3 value) {
	position = value;
}

Vector3 Object::getSize() const {
	return size;
}

void Object::setSize(Vector3 value) {
	size = normaliseSize(value);
}

BoundingBox Object::getBounds() const {
	const Vector3 halfSize {
		size.x * 0.5f,
		size.y * 0.5f,
		size.z * 0.5f
	};

	return {
		{position.x - halfSize.x, position.y - halfSize.y, position.z - halfSize.z},
		{position.x + halfSize.x, position.y + halfSize.y, position.z + halfSize.z}
	};
}

bool Object::isActive() const {
	return active;
}

void Object::setActive(bool value) {
	active = value;
}

bool Object::isVisible() const {
	return visible;
}

void Object::setVisible(bool value) {
	visible = value;
}

bool Object::isCollidable() const {
	return collidable;
}

void Object::setCollidable(bool value) {
	collidable = value;
}
