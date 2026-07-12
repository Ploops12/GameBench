#include <cmath>
#include "Pickup.hxx"
#include <raymath.h>

Pickup::Pickup()
	: Object({}, 0.5f) {
}

Pickup::Pickup(Vector3 position, ReagentType type, int amount)
	: Object(position, 0.55f), reagent(type), amount(amount) {
}

void Pickup::update(float dt) {
	bobTime += dt;
}

void Pickup::draw3D(const Camera3D&) const {
	if (!active) {
		return;
	}

	const ReagentDescriptor& descriptor = reagent.descriptor();
	const float bob = 0.35f + std::sin(bobTime * 3.5f) * 0.08f;
	const Vector3 drawPos {position.x, position.y + bob, position.z};

	DrawSphereWires(drawPos, 0.22f, 8, 8, descriptor.color);
	DrawLine3D(
		Vector3 {drawPos.x - 0.18f, drawPos.y - 0.25f, drawPos.z},
		Vector3 {drawPos.x + 0.18f, drawPos.y + 0.25f, drawPos.z},
		descriptor.color
	);
	DrawLine3D(
		Vector3 {drawPos.x, drawPos.y - 0.25f, drawPos.z - 0.18f},
		Vector3 {drawPos.x, drawPos.y + 0.25f, drawPos.z + 0.18f},
		descriptor.color
	);
}

ReagentType Pickup::getType() const {
	return reagent.getType();
}

int Pickup::getAmount() const {
	return amount;
}

bool Pickup::tryCollect(const Vector3& collectorPosition, float radius) {
	if (!active) {
		return false;
	}

	if (Vector3Distance(position, collectorPosition) <= radius) {
		active = false;
		return true;
	}

	return false;
}
