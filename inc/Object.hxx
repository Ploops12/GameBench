#pragma once

#include <raylib.h>

class Object {
public:
	explicit Object(Vector3 position = {});
	virtual ~Object() = default;

	virtual void draw3D() const;
	Vector3 getPosition() const;
	void setPosition(Vector3 value);
	float getCollisionRadius() const;
	void setCollisionRadius(float value);
	bool isActive() const;
	void setActive(bool value);

protected:
	Vector3 position {};
	float collisionRadius {0.4f};
	bool active {true};
};
