#include "Pickup.hxx"

#include <algorithm>
#include <cmath>

#include <raymath.h>

namespace {
constexpr float PickupRadius = 0.28f;
constexpr float BobHeight = 0.13f;
constexpr float BobSpeed = 2.5f;
constexpr float Pi = 3.14159265358979323846f;

Color colorFor(ReagentType type) {
	switch (type) {
	case ReagentType::GraveSalt:
		return Color {194, 239, 230, 255};
	case ReagentType::ChurchGlass:
		return Color {73, 207, 218, 255};
	case ReagentType::WidowNettle:
		return Color {154, 224, 91, 255};
	case ReagentType::PlagueHoney:
		return Color {245, 188, 62, 255};
	case ReagentType::MothDust:
		return Color {194, 125, 230, 255};
	case ReagentType::BlackWax:
		return Color {236, 90, 107, 255};
	case ReagentType::Count:
		break;
	}

	return RAYWHITE;
}

Vector3 offsetFromAngle(float angle, float radius, float height = 0.0f) {
	return Vector3 {std::cos(angle) * radius, height, std::sin(angle) * radius};
}
}

Pickup::Pickup(Vector3 initialPosition, ReagentType initialType, int initialAmount)
	: Object(initialPosition, PickupRadius), type(initialType), amount(std::max(1, initialAmount)) {
}

void Pickup::update(float deltaTime) {
	if (!isActive()) {
		return;
	}

	bobTime += std::max(0.0f, deltaTime) * BobSpeed;
	spin += std::max(0.0f, deltaTime) * 1.4f;
}

void Pickup::draw(const Camera3D&) const {
	if (!isActive() || !isDrawable()) {
		return;
	}

	const Color color = colorFor(type);
	const Color ghostColor {color.r, color.g, color.b, 92};
	const Vector3 visualPosition = getVisualPosition();
	const Vector3 groundMarker {position.x, position.y + 0.012f, position.z};

	DrawSphereWires(visualPosition, 0.18f, 5, 6, color);
	DrawCircle3D(groundMarker, 0.31f, Vector3 {1.0f, 0.0f, 0.0f}, 90.0f, ghostColor);

	for (int point = 0; point < 6; ++point) {
		const float angle = spin + static_cast<float>(point) * Pi / 3.0f;
		const Vector3 outer = Vector3Add(visualPosition, offsetFromAngle(angle, 0.25f));
		const Vector3 inner = Vector3Add(visualPosition, offsetFromAngle(angle + Pi / 6.0f, 0.08f, 0.05f));
		DrawLine3D(inner, outer, ghostColor);
	}

	switch (type) {
	case ReagentType::GraveSalt:
		DrawCubeWires(visualPosition, 0.22f, 0.34f, 0.22f, color);
		break;
	case ReagentType::ChurchGlass:
		DrawLine3D(Vector3Add(visualPosition, Vector3 {-0.18f, -0.14f, 0.0f}),
			Vector3Add(visualPosition, Vector3 {0.0f, 0.26f, 0.0f}), color);
		DrawLine3D(Vector3Add(visualPosition, Vector3 {0.0f, 0.26f, 0.0f}),
			Vector3Add(visualPosition, Vector3 {0.18f, -0.14f, 0.0f}), color);
		break;
	case ReagentType::WidowNettle:
		for (int thorn = 0; thorn < 3; ++thorn) {
			const float angle = spin + static_cast<float>(thorn) * (2.0f * Pi) / 3.0f;
			DrawLine3D(visualPosition,
				Vector3Add(visualPosition, offsetFromAngle(angle, 0.26f, 0.14f)), color);
		}
		break;
	case ReagentType::PlagueHoney:
		DrawSphereWires(Vector3Add(visualPosition, Vector3 {0.0f, 0.12f, 0.0f}), 0.13f, 4, 5, color);
		break;
	case ReagentType::MothDust:
		DrawLine3D(Vector3Add(visualPosition, Vector3 {-0.27f, 0.05f, 0.0f}), visualPosition, color);
		DrawLine3D(visualPosition, Vector3Add(visualPosition, Vector3 {0.27f, 0.05f, 0.0f}), color);
		break;
	case ReagentType::BlackWax:
		DrawCubeWires(Vector3Add(visualPosition, Vector3 {0.0f, -0.04f, 0.0f}), 0.24f, 0.18f, 0.24f, color);
		DrawLine3D(visualPosition, Vector3Add(visualPosition, Vector3 {0.0f, 0.27f, 0.0f}), color);
		break;
	case ReagentType::Count:
		break;
	}
}

ReagentType Pickup::getType() const {
	return type;
}

int Pickup::getAmount() const {
	return amount;
}

Vector3 Pickup::getVisualPosition() const {
	return Vector3 {
		position.x,
		position.y + 0.38f + std::sin(bobTime) * BobHeight,
		position.z
	};
}

bool Pickup::collectIfNearby(Vector3 collectorPosition, float collectionRadius) {
	if (!isActive() || collectionRadius < 0.0f) {
		return false;
	}

	const Vector3 planarCollector {collectorPosition.x, position.y, collectorPosition.z};
	if (!CheckCollisionSpheres(position, collisionRadius, planarCollector, collectionRadius)) {
		return false;
	}

	setActive(false);
	setDrawable(false);
	setCollisionEnabled(false);
	return true;
}
