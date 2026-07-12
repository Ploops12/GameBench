#include "Pickup.hxx"
#include "Scenery.hxx"

#include <raylib.h>

#include <algorithm>
#include <cmath>

namespace {
	constexpr Color OUTLINE{220, 228, 214, 255};
	constexpr Color DARK{18, 22, 30, 255};
	constexpr Color WOOD{54, 58, 67, 255};
	constexpr Color HOUSE_RED{192, 62, 76, 255};
	constexpr Color TREE_GREEN{106, 190, 120, 255};

	float flatDistance(Vector3 a, Vector3 b) {
		const float dx = a.x - b.x;
		const float dz = a.z - b.z;
		return std::sqrt(dx * dx + dz * dz);
	}
}

Pickup::Pickup(ReagentType type, Vector3 initialPosition)
	: Object(initialPosition), reagent(type) {
}

void Pickup::update(float deltaTime) {
	animationTime += deltaTime;
}

void Pickup::draw() const {
	const ReagentDefinition& definition = GetReagentDefinition(reagent);
	const float bob = std::sin(animationTime * 2.4f + position.x) * 0.08f;
	const Vector3 drawPosition{position.x, position.y + bob, position.z};
	DrawSphere(drawPosition, 0.2f, ColorAlpha(definition.color, 0.35f));
	DrawSphereWires(drawPosition, 0.27f, 8, 6, definition.color);
	DrawCircle3D({drawPosition.x, 0.03f, drawPosition.z}, 0.34f, {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(definition.color, 0.7f));
	DrawLine3D({drawPosition.x - 0.18f, drawPosition.y, drawPosition.z}, {drawPosition.x + 0.18f, drawPosition.y, drawPosition.z}, definition.color);
	DrawLine3D({drawPosition.x, drawPosition.y, drawPosition.z - 0.18f}, {drawPosition.x, drawPosition.y, drawPosition.z + 0.18f}, definition.color);
}

bool Pickup::canCollect(Vector3 playerPosition, float playerRadius) const {
	return flatDistance(position, playerPosition) <= playerRadius + 0.65f;
}

ReagentType Pickup::type() const {
	return reagent;
}

Scenery::Scenery(SceneryKind kind, Vector3 initialPosition, Vector3 dimensions, bool isSolid)
	: Object(initialPosition), sceneryKind(kind), size(dimensions), solid(isSolid) {
	collisionBounds = {
		{position.x - size.x * 0.5f, position.y, position.z - size.z * 0.5f},
		{position.x + size.x * 0.5f, position.y + size.y, position.z + size.z * 0.5f}
	};
}

const BoundingBox& Scenery::bounds() const {
	return collisionBounds;
}

bool Scenery::isSolid() const {
	return solid;
}

void Scenery::draw() const {
	switch (sceneryKind) {
		case SceneryKind::House: {
			const Vector3 body{position.x, position.y + size.y * 0.5f, position.z};
			const Vector3 roof{position.x, position.y + size.y + 0.35f, position.z};
			DrawCubeV(body, size, ColorAlpha(DARK, 0.92f));
			DrawCubeWiresV(body, size, OUTLINE);
			DrawCubeV(roof, {size.x * 1.08f, 0.5f, size.z * 1.08f}, ColorAlpha(WOOD, 0.9f));
			DrawCubeWiresV(roof, {size.x * 1.08f, 0.5f, size.z * 1.08f}, OUTLINE);
			DrawCubeV({position.x, position.y + size.y * 0.42f, position.z - size.z * 0.51f}, {size.x * 0.2f, size.y * 0.24f, 0.04f}, HOUSE_RED);
			DrawCubeV({position.x - size.x * 0.3f, position.y + size.y * 0.48f, position.z - size.z * 0.51f}, {size.x * 0.12f, size.y * 0.14f, 0.04f}, TREE_GREEN);
			break;
		}
		case SceneryKind::Fence: {
			DrawCubeV({position.x, position.y + size.y * 0.52f, position.z}, {size.x, size.y * 0.18f, size.z * 0.18f}, WOOD);
			DrawCubeV({position.x, position.y + size.y * 0.27f, position.z}, {size.x, size.y * 0.14f, size.z * 0.14f}, WOOD);
			DrawCubeWiresV({position.x, position.y + size.y * 0.52f, position.z}, {size.x, size.y * 0.18f, size.z * 0.18f}, OUTLINE);
			DrawCubeWiresV({position.x, position.y + size.y * 0.27f, position.z}, {size.x, size.y * 0.14f, size.z * 0.14f}, OUTLINE);
			for (int i = -1; i <= 1; i += 2) {
				DrawCubeV({position.x + size.x * 0.38f * static_cast<float>(i), position.y + size.y * 0.5f, position.z}, {0.18f, size.y, 0.28f}, DARK);
				DrawCubeWiresV({position.x + size.x * 0.38f * static_cast<float>(i), position.y + size.y * 0.5f, position.z}, {0.18f, size.y, 0.28f}, OUTLINE);
			}
			break;
		}
		case SceneryKind::Tree: {
			DrawCylinder({position.x, position.y + size.y * 0.38f, position.z}, size.x * 0.16f, size.x * 0.22f, size.y * 0.76f, 7, DARK);
			DrawCylinderWires({position.x, position.y + size.y * 0.38f, position.z}, size.x * 0.16f, size.x * 0.22f, size.y * 0.76f, 7, OUTLINE);
			for (int i = 0; i < 4; ++i) {
				const float angle = static_cast<float>(i) * PI * 0.5f;
				const Vector3 branchStart{position.x, position.y + size.y * 0.48f, position.z};
				const Vector3 branchEnd{position.x + std::cos(angle) * size.x, position.y + size.y * (0.7f + 0.04f * static_cast<float>(i)), position.z + std::sin(angle) * size.x};
				DrawLine3D(branchStart, branchEnd, OUTLINE);
			}
			DrawCircle3D({position.x, 0.03f, position.z}, size.x * 0.6f, {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(TREE_GREEN, 0.7f));
			break;
		}
		case SceneryKind::Lantern: {
			DrawCylinder({position.x, position.y + size.y * 0.5f, position.z}, size.x * 0.12f, size.x * 0.16f, size.y, 6, WOOD);
			DrawCylinderWires({position.x, position.y + size.y * 0.5f, position.z}, size.x * 0.12f, size.x * 0.16f, size.y, 6, OUTLINE);
			DrawSphere({position.x, position.y + size.y * 0.75f, position.z}, size.x * 0.22f, ColorAlpha({242, 178, 82, 255}, 0.8f));
			DrawSphereWires({position.x, position.y + size.y * 0.75f, position.z}, size.x * 0.3f, 8, 6, OUTLINE);
			break;
		}
		case SceneryKind::Shrine: {
			DrawCubeV({position.x, position.y + size.y * 0.2f, position.z}, {size.x, size.y * 0.4f, size.z}, WOOD);
			DrawCubeWiresV({position.x, position.y + size.y * 0.2f, position.z}, {size.x, size.y * 0.4f, size.z}, OUTLINE);
			DrawCylinder({position.x, position.y + size.y * 0.58f, position.z}, size.x * 0.45f, size.x * 0.45f, size.y * 0.4f, 6, DARK);
			DrawCylinderWires({position.x, position.y + size.y * 0.58f, position.z}, size.x * 0.45f, size.x * 0.45f, size.y * 0.4f, 6, OUTLINE);
			DrawCircle3D({position.x, position.y + size.y * 0.8f, position.z}, size.x * 0.25f, {0.0f, 0.0f, 1.0f}, 0.0f, HOUSE_RED);
			break;
		}
	}
}
