#pragma once

#include <raylib.h>

class Object {
public:
	explicit Object(Vector3 position = {}, float collisionRadius = 0.5f);
	virtual ~Object() = default;

	virtual void update(float dt);
	void draw(const Camera3D& camera) const;

	[[nodiscard]] virtual BoundingBox bounds() const;
	[[nodiscard]] Vector3 position() const;
	void setPosition(Vector3 position);
	[[nodiscard]] float collisionRadius() const;

	[[nodiscard]] bool isActive() const;
	void setActive(bool active);
	[[nodiscard]] bool isDrawEnabled() const;
	void setDrawEnabled(bool enabled);
	[[nodiscard]] bool isCollisionEnabled() const;
	void setCollisionEnabled(bool enabled);

protected:
	virtual void onDraw(const Camera3D& camera) const;

	Vector3 position_ {};
	float collisionRadius_ {0.5f};
	bool active_ {true};
	bool drawEnabled_ {true};
	bool collisionEnabled_ {true};
};
