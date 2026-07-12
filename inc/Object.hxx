#pragma once

#include <raylib.h>
#include <raymath.h>

class Object {
public:
	virtual ~Object() = default;
	virtual void update(float) {}
	virtual void draw() const {}

	Vector3 getPosition() const { return position; }
	void setPosition(Vector3 value) { position = value; }
	bool isActive() const { return active; }

protected:
	Vector3 position{};
	Vector3 halfSize{0.5f, 0.5f, 0.5f};
	bool active{true};
};
