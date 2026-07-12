#pragma once

#include <raylib.h>

class Object {
public:
	Object(Vector3 position = {}, float collisionRadius = 0.5f);
	virtual ~Object() = default;

	virtual void update(float dt);
	virtual void draw3D(const Camera3D& camera) const = 0;

	bool isActive() const;
	void setActive(bool value);

	bool isCollidable() const;
	void setCollidable(bool value);

	float getCollisionRadius() const;
	const Vector3& getPosition() const;
	void setPosition(const Vector3& value);
protected:
	Vector3 position {};
	float collisionRadius {0.5f};
	bool active {true};
	bool collidable {false};
};
