#include "Pickup.hxx"

#include <cmath>

#include <raymath.h>

Pickup::Pickup(Vector3 initialPosition, ReagentType initialType)
	: Object(initialPosition, 0.35f), type(initialType), baseHeight(initialPosition.y) {
}

void Pickup::update(float deltaTime) {
	bobTime += deltaTime;
	position.y = baseHeight + 0.34f + std::sin(bobTime * 2.8f) * 0.09f;
}

void Pickup::draw3D() const {
	if (collected || !visible) {
		return;
	}

	const Color color = getReagentDefinition(type).color;
	DrawCube(position, 0.22f, 0.22f, 0.22f, Fade(color, 0.55f));
	DrawCubeWires(position, 0.26f, 0.26f, 0.26f, RAYWHITE);
	DrawLine3D({position.x, position.y - 0.22f, position.z}, {position.x, position.y + 0.32f, position.z}, color);
	DrawCircle3D({position.x, baseHeight + 0.025f, position.z}, 0.28f, {1.0f, 0.0f, 0.0f}, 90.0f, Fade(color, 0.65f));
}

ReagentType Pickup::getType() const {
	return type;
}

bool Pickup::tryCollect(Vector3 playerPosition, float collectionRadius) {
	if (collected || Vector3Distance(position, playerPosition) > collectionRadius) {
		return false;
	}

	collected = true;
	visible = false;
	return true;
}

bool Pickup::isCollected() const {
	return collected;
}
