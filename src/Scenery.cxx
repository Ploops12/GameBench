#include "Scenery.hxx"

bool Scenery::collides(Vector3 point, float radius) const {
	BoundingBox box{Vector3Subtract(position, halfSize), Vector3Add(position, halfSize)};
	BoundingBox actor{Vector3{point.x - radius, point.y - radius, point.z - radius}, Vector3{point.x + radius, point.y + radius, point.z + radius}};
	return CheckCollisionBoxes(actor, box);
}
