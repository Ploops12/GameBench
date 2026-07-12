#pragma once

#include <raylib.h>

class Object {
public:
	Object();
	Object(Vector3 startPosition, float radius, bool isCollidable);
	virtual ~Object() = default;

	virtual void update(float dt);
	virtual void draw3D(const Camera3D& camera) const;
	virtual void draw2D() const;

	Vector3 getPosition() const;
	void setPosition(Vector3 newPosition);

	float getCollisionRadius() const;
	void setCollisionRadius(float radius);

	bool isActive() const;
	void setActive(bool value);

	bool isCollidable() const;
	void setCollidable(bool value);

	virtual BoundingBox getBoundingBox() const;

protected:
	Vector3 position;
	float collisionRadius;
	bool active;
	bool collidable;
};
