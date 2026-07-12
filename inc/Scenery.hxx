#pragma once

#include "Object.hxx"

enum class SceneryKind {
	House,
	Fence,
	Tree,
	Lantern,
	Shrine
};

class Scenery : public Object {
public:
	Scenery(SceneryKind kind, Vector3 position, Vector3 size, bool solid = true);

	void draw() const override;
	const BoundingBox& bounds() const;
	bool isSolid() const;

private:
	SceneryKind sceneryKind{SceneryKind::House};
	Vector3 size{1.0f, 1.0f, 1.0f};
	BoundingBox collisionBounds{};
	bool solid{true};
};
