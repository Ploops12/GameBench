#include "Scenery.hxx"

Scenery::Scenery(Vector3 position, Vector3 size, Color color, bool collidable) :
	Object(position, 0.5f), size(size), color(color) {
	solid = collidable;
}

BoundingBox Scenery::getBounds() const {
	return BoundingBox{
		Vector3{position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z - size.z * 0.5f},
		Vector3{position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z + size.z * 0.5f}
	};
}

void Scenery::draw3D() const {
	DrawCubeV(position, size, Fade(color, 0.08f));
	DrawCubeWiresV(position, size, color);
	if (size.y > 2.0f) {
		const float roofY = position.y + size.y * 0.5f;
		DrawLine3D(Vector3{position.x - size.x * 0.5f, roofY, position.z - size.z * 0.5f},
			Vector3{position.x, roofY + 1.4f, position.z - size.z * 0.5f}, color);
		DrawLine3D(Vector3{position.x + size.x * 0.5f, roofY, position.z - size.z * 0.5f},
			Vector3{position.x, roofY + 1.4f, position.z - size.z * 0.5f}, color);
	}
}
