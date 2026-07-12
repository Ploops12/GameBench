#pragma once

#include "Object.hxx"

enum class SceneryType {
	Building,
	Fence,
	Lantern,
	StandingStone
};

class Scenery : public Object {
public:
	Scenery(SceneryType type, Vector3 position, Vector3 size, bool collidable = true);

	void draw3D() const override;
	BoundingBox getBounds() const;
	SceneryType getType() const;

private:
	SceneryType type {SceneryType::Building};
	Vector3 size {1.0f, 1.0f, 1.0f};
	Color color {DARKGRAY};
};
