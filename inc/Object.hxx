#pragma once

#include <raylib.h>

class Object {
public:
	explicit Object(Vector3 position = {}, float collisionRadius = 0.0f);
	virtual ~Object() = default;

	virtual void update(float deltaTime);
	virtual void draw3D() const;

	const Vector3& getPosition() const;
	void setPosition(Vector3 newPosition);
	float getCollisionRadius() const;
	bool isCollisionEnabled() const;
	void setCollisionEnabled(bool enabled);
	bool isVisible() const;
	void setVisible(bool shouldDraw);

protected:
	Vector3 position {};
	float collisionRadius {0.0f};
	bool collisionEnabled {true};
	bool visible {true};
};
