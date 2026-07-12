#pragma once

#include "Object.hxx"

class Scenery : public Object {
public:
	explicit Scenery(BoundingBox bounds = {});
	void draw() const override;
	const BoundingBox& bounds() const { return collisionBounds; }

private:
	BoundingBox collisionBounds {};
};
