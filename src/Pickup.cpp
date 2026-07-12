#include "Pickup.hxx"

#include <cmath>

Pickup::Pickup(Vector3 initialPosition, ReagentType value) : Object(initialPosition), type(value) {
	collisionRadius = 0.8f;
	bobOffset = initialPosition.x + initialPosition.z;
}

void Pickup::update(float time) {
	position.y = 0.28f + std::sin(time * 3.0f + bobOffset) * 0.10f;
}

void Pickup::draw3D() const {
	if (!active) return;
	Color color = getReagent(type).color;
	DrawCircle3D(position, 0.22f, {1.0f, 0.0f, 0.0f}, 90.0f, color);
	DrawSphereWires(position, 0.16f, 6, 4, color);
	DrawLine3D({position.x, position.y - 0.24f, position.z}, {position.x, 0.03f, position.z}, Fade(color, 0.65f));
}

ReagentType Pickup::getType() const { return type; }
