#pragma once

#include "Object.hxx"

enum class SceneryShape {
	House,
	Fence,
	Tree,
	Stone
};

class Scenery : public Object {
public:
	Scenery();
	Scenery(Vector3 startPosition, Vector3 size, SceneryShape shape, bool solid, Color tint);

	void draw3D(const Camera3D& camera) const override;
	BoundingBox getBoundingBox() const override;

	Vector3 getSize() const;
	SceneryShape getShape() const;
	Color getTint() const;

private:
	Vector3 size;
	SceneryShape shape;
	Color tint;
};
