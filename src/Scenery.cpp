#include "Scenery.hxx"

#include <algorithm>

Scenery::Scenery(SceneryType initialType, Vector3 initialPosition, Vector3 initialSize, bool collidable)
	: Object(initialPosition, std::max(initialSize.x, initialSize.z) * 0.5f), type(initialType), size(initialSize) {
	collisionEnabled = collidable;

	switch (type) {
	case SceneryType::Building:
		color = {22, 30, 44, 255};
		break;
	case SceneryType::Fence:
		color = {72, 92, 86, 255};
		break;
	case SceneryType::Lantern:
		color = {255, 168, 76, 255};
		break;
	case SceneryType::StandingStone:
		color = {80, 89, 118, 255};
		break;
	}
}

void Scenery::draw3D() const {
	if (!visible) {
		return;
	}

	switch (type) {
	case SceneryType::Building:
		DrawCube(position, size.x, size.y, size.z, Fade(color, 0.9f));
		DrawCubeWires(position, size.x, size.y, size.z, {137, 177, 194, 255});
		DrawLine3D({position.x - size.x * 0.5f, position.y + size.y * 0.15f, position.z - size.z * 0.51f},
			{position.x + size.x * 0.5f, position.y + size.y * 0.15f, position.z - size.z * 0.51f}, {137, 177, 194, 180});
		break;
	case SceneryType::Fence:
		DrawCube(position, size.x, size.y * 0.18f, size.z, color);
		DrawCubeWires(position, size.x, size.y * 0.18f, size.z, RAYWHITE);
		for (float offset = -size.x * 0.45f; offset <= size.x * 0.45f; offset += 1.1f) {
			const Vector3 post {position.x + offset, position.y + size.y * 0.35f, position.z};
			DrawCube(post, 0.09f, size.y, 0.09f, color);
			DrawCubeWires(post, 0.1f, size.y, 0.1f, {177, 214, 207, 255});
		}
		break;
	case SceneryType::Lantern: {
		const Vector3 post {position.x, position.y + size.y * 0.25f, position.z};
		const Vector3 lantern {position.x, position.y + size.y * 0.72f, position.z};
		DrawCube(post, 0.11f, size.y, 0.11f, {98, 109, 118, 255});
		DrawCubeWires(post, 0.14f, size.y, 0.14f, RAYWHITE);
		DrawSphere(lantern, 0.18f, color);
		DrawSphereWires(lantern, 0.25f, 6, 4, RAYWHITE);
		break;
	}
	case SceneryType::StandingStone:
		DrawCube(position, size.x, size.y, size.z, color);
		DrawCubeWires(position, size.x, size.y, size.z, {167, 183, 237, 255});
		DrawCircle3D({position.x, 0.025f, position.z}, size.x * 0.72f, {1.0f, 0.0f, 0.0f}, 90.0f, {167, 183, 237, 180});
		break;
	}
}

BoundingBox Scenery::getBounds() const {
	return {
		{position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z - size.z * 0.5f},
		{position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z + size.z * 0.5f}
	};
}

SceneryType Scenery::getType() const {
	return type;
}
