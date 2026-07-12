#pragma once

#include "Object.hxx"

class Scenery : public Object {
public:
	Scenery(BoundingBox bounds, Color color, bool collidable = true);

	void draw3D() const override;
	const BoundingBox& getBounds() const;
	bool isCollidable() const;

private:
	BoundingBox bounds {};
	Color color {};
	bool collidable {};
};
