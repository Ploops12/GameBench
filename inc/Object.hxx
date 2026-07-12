#pragma once

#include <raylib.h>

class Object {
public:
	explicit Object(Vector3 position = {}) : position(position) {}
	virtual ~Object() = default;
	virtual void update(float) {}
	virtual void draw() const {}

	Vector3 position {};
	bool active{true};
	bool visible{true};
	bool collidable{false};
	float collisionRadius{0.5f};
};
