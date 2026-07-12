#include "Scenery.hxx"

Scenery::Scenery(BoundingBox initialBounds, Color initialColor, bool initialCollidable)
	: Object({(initialBounds.min.x + initialBounds.max.x) * 0.5f, initialBounds.min.y, (initialBounds.min.z + initialBounds.max.z) * 0.5f}),
	  bounds(initialBounds), color(initialColor), collidable(initialCollidable) {}

void Scenery::draw3D() const {
	DrawBoundingBox(bounds, color);
	Vector3 center {(bounds.min.x + bounds.max.x) * 0.5f, (bounds.min.y + bounds.max.y) * 0.5f, (bounds.min.z + bounds.max.z) * 0.5f};
	DrawCube(center, bounds.max.x - bounds.min.x, bounds.max.y - bounds.min.y, bounds.max.z - bounds.min.z, Fade(color, 0.10f));
}

const BoundingBox& Scenery::getBounds() const { return bounds; }
bool Scenery::isCollidable() const { return collidable; }
