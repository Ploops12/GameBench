#pragma once

#include <raylib.h>
#include "Object.hxx"

class Scenery : public Object {
public:
	Scenery(Vector3 center = {}, Vector3 size = {1.0f, 1.0f, 1.0f}, bool collidable = true);
	BoundingBox GetBounds() const;
	bool IsCollidable() const { return collidable; }
	void Draw() const override;

private:
	Vector3 size{1.0f, 1.0f, 1.0f};
	bool collidable{true};
};
