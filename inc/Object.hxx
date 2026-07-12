#pragma once

#include <raylib.h>

class Object {
public:
	explicit Object(Vector3 position = {}, float collisionRadius = 0.5f);
	virtual ~Object() = default;

	virtual void update(float dt);
	virtual void draw3D() const = 0;

	Vector3 getPosition() const { return position; }
	void setPosition(Vector3 value) { position = value; }
	float getCollisionRadius() const { return collisionRadius; }
	bool isActive() const { return active; }
	void setActive(bool value) { active = value; }
	bool isSolid() const { return solid; }
	void setSolid(bool value) { solid = value; }

protected:
	Vector3 position {};
	float collisionRadius {0.5f};
	bool active {true};
	bool solid {false};
};
