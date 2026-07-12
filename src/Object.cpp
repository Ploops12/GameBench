#include "Object.hxx"

#include <algorithm>

Object::Object(Vector3 position, float collisionRadius)
	: position_(position), collisionRadius_(std::max(0.0f, collisionRadius)) {
}

void Object::update(float) {
}

void Object::draw(const Camera3D& camera) const {
	if (active_ && drawEnabled_) {
		onDraw(camera);
	}
}

BoundingBox Object::bounds() const {
	return {
		{position_.x - collisionRadius_, position_.y - collisionRadius_, position_.z - collisionRadius_},
		{position_.x + collisionRadius_, position_.y + collisionRadius_, position_.z + collisionRadius_}
	};
}

Vector3 Object::position() const {
	return position_;
}

void Object::setPosition(Vector3 position) {
	position_ = position;
}

float Object::collisionRadius() const {
	return collisionRadius_;
}

bool Object::isActive() const {
	return active_;
}

void Object::setActive(bool active) {
	active_ = active;
}

bool Object::isDrawEnabled() const {
	return drawEnabled_;
}

void Object::setDrawEnabled(bool enabled) {
	drawEnabled_ = enabled;
}

bool Object::isCollisionEnabled() const {
	return collisionEnabled_;
}

void Object::setCollisionEnabled(bool enabled) {
	collisionEnabled_ = enabled;
}

void Object::onDraw(const Camera3D&) const {
}
