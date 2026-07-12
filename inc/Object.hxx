#pragma once

#include <raylib.h>

class Object {
public:
	virtual ~Object() = default;
	virtual void update(float dt) { (void)dt; }
	virtual void draw() const {}

	Vector3 position{};
	float collisionRadius{0.5f};
	bool collisionEnabled{true};
	bool drawingEnabled{true};
};
