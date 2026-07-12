#include <cmath>
#include "Pickup.hxx"

Pickup::Pickup() : Object(), type(ReagentType::GraveSalt), amount(1), bobTime(0.0f) {}

Pickup::Pickup(Vector3 startPosition, ReagentType pickupType, int pickupAmount)
	: Object(startPosition, 0.55f, false), type(pickupType), amount(pickupAmount), bobTime(0.0f) {}

void Pickup::update(float dt) {
	bobTime += dt;
	position.y = 0.5f + std::sin(bobTime * 4.0f) * 0.1f;
}

void Pickup::draw3D(const Camera3D&) const {
	const ReagentInfo& info = getReagentInfo(type);
	DrawSphere(position, 0.18f, info.color);
	DrawSphereWires(position, 0.22f, 8, 8, WHITE);
	DrawCircle3D({position.x, 0.03f, position.z}, 0.4f, {1.0f, 0.0f, 0.0f}, 90.0f, Fade(info.color, 0.7f));
}

ReagentType Pickup::getType() const {
	return type;
}

int Pickup::getAmount() const {
	return amount;
}
