#pragma once

#include "Object.hxx"

class Scenery : public Object {
public:
	Scenery(Vector3 position, Vector3 size, Color color, bool collidable = true);

	void draw3D() const override;
	BoundingBox getBounds() const;
	Vector3 getSize() const { return size; }

private:
	Vector3 size {};
	Color color {GRAY};
};
