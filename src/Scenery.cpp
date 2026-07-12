#include "Scenery.hxx"

Scenery::Scenery() : Object(), size {1.0f, 1.0f, 1.0f}, shape(SceneryShape::Stone), tint(GRAY) {}

Scenery::Scenery(Vector3 startPosition, Vector3 worldSize, SceneryShape worldShape, bool solid, Color worldTint)
	: Object(startPosition, worldSize.x * 0.5f, solid), size(worldSize), shape(worldShape), tint(worldTint) {}

void Scenery::draw3D(const Camera3D&) const {
	switch (shape) {
		case SceneryShape::House:
			DrawCubeV({position.x, position.y + size.y * 0.5f, position.z}, size, Fade(tint, 0.28f));
			DrawCubeWiresV({position.x, position.y + size.y * 0.5f, position.z}, size, tint);
			break;
		case SceneryShape::Fence:
			DrawCubeV({position.x, position.y + size.y * 0.5f, position.z}, size, Fade(tint, 0.15f));
			DrawCubeWiresV({position.x, position.y + size.y * 0.5f, position.z}, size, tint);
			break;
		case SceneryShape::Tree:
			DrawCylinder({position.x, position.y + size.y * 0.35f, position.z}, 0.08f, 0.05f, size.y * 0.7f, 6, tint);
			DrawSphere({position.x, position.y + size.y * 0.85f, position.z}, size.x * 0.35f, Fade(tint, 0.2f));
			DrawSphereWires({position.x, position.y + size.y * 0.85f, position.z}, size.x * 0.38f, 8, 8, tint);
			break;
		case SceneryShape::Stone:
			DrawCubeV({position.x, position.y + size.y * 0.5f, position.z}, size, Fade(tint, 0.18f));
			DrawCubeWiresV({position.x, position.y + size.y * 0.5f, position.z}, size, tint);
			break;
	}
}

BoundingBox Scenery::getBoundingBox() const {
	return {
		{position.x - size.x * 0.5f, position.y, position.z - size.z * 0.5f},
		{position.x + size.x * 0.5f, position.y + size.y, position.z + size.z * 0.5f}
	};
}

Vector3 Scenery::getSize() const {
	return size;
}

SceneryShape Scenery::getShape() const {
	return shape;
}

Color Scenery::getTint() const {
	return tint;
}
