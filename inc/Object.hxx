#pragma once

#include <raylib.h>

class Object {
	// Base for world objects. Position is the centre of the object's local box.
public:
	explicit Object(Vector3 position = {}, Vector3 size = {1.0f, 1.0f, 1.0f});
	virtual ~Object() = default;

	virtual void update(float deltaTime);
	virtual void draw3D(const Camera3D& camera) const;

	Vector3 getPosition() const;
	void setPosition(Vector3 value);
	Vector3 getSize() const;
	void setSize(Vector3 value);
	BoundingBox getBounds() const;

	bool isActive() const;
	void setActive(bool value);
	bool isVisible() const;
	void setVisible(bool value);
	bool isCollidable() const;
	void setCollidable(bool value);

protected:
	Vector3 position {};
	Vector3 size {1.0f, 1.0f, 1.0f};
	bool active {true};
	bool visible {true};
	bool collidable {true};
};
