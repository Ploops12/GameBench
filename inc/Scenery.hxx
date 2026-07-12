#pragma once

// Box-based line-art scenery used for arena landmarks and player collision.

#include "Object.hxx"

#include <string>

class Scenery : public Object {
public:
	Scenery() = default;
	Scenery(Vector3 center, Vector3 size, Color lineColor, bool collidable = true, std::string label = {});

	void draw(const Camera3D& camera) const override;
	BoundingBox bounds() const;
	bool containsXZ(Vector3 point, float radius) const;

	Vector3 size{1.0f, 1.0f, 1.0f};
	Color color{DARKGRAY};
	std::string name{};
};
