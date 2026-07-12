#pragma once

#include "Object.hxx"

class Scenery : public Object {
public:
	Scenery(Vector3 position, Vector3 size, Color color) : color(color) { this->position = position; halfSize = Vector3Scale(size, 0.5f); }
	void draw() const override { DrawCube(position, halfSize.x * 2.0f, halfSize.y * 2.0f, halfSize.z * 2.0f, color); DrawCubeWires(position, halfSize.x * 2.0f, halfSize.y * 2.0f, halfSize.z * 2.0f, BLACK); }
	bool collides(Vector3 point, float radius) const;

private:
	Color color;
};
