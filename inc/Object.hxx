#pragma once

#include <raylib.h>

class Object {
public:
	Object(Vector3 position = {}, float collisionRadius = 0.0f);
	virtual ~Object() = default;

	virtual void update(float deltaTime);
	virtual void draw(const Camera3D& camera) const;

	const Vector3& getPosition() const;
	void setPosition(Vector3 value);
	float getCollisionRadius() const;

	bool isActive() const;
	void setActive(bool value);
	bool isDrawable() const;
	void setDrawable(bool value);
	bool hasCollision() const;
	void setCollisionEnabled(bool value);
	bool overlapsSphere(Vector3 center, float radius) const;

protected:
	Vector3 position {};
	float collisionRadius {0.0f};
	bool active {true};
	bool drawable {true};
	bool collisionEnabled {true};
};
