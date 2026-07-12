#include "Scenery.hxx"

Scenery::Scenery(Vector3 center, Vector3 sizeValue, bool collidableValue)
	: size(sizeValue), collidable(collidableValue) {
	position = center;
	radius = size.x > size.z ? size.x : size.z;
}

BoundingBox Scenery::GetBounds() const {
	return {{position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z - size.z * 0.5f},
		{position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z + size.z * 0.5f}};
}

void Scenery::Draw() const {
	const Color fill = collidable ? Color{28, 34, 42, 255} : Color{36, 48, 44, 255};
	DrawCube(position, size.x, size.y, size.z, fill);
	DrawCubeWires(position, size.x, size.y, size.z, collidable ? BEIGE : DARKGREEN);
}
