#pragma once

#include "Object.hxx"

enum class SceneryKind {
	SolidBox,
	Fence,
	DeadTree,
	Gallows,
	Lantern,
	StandingStone
};

class Scenery : public Object {
	// Collidable boxes and sparse non-collidable line-art props.
public:
	Scenery(SceneryKind kind, Vector3 position, Vector3 size, Color color, bool collidable = true);

	void draw3D(const Camera3D& camera) const override;
	SceneryKind getKind() const;
	Color getColor() const;

private:
	SceneryKind kind;
	Color color;
};
