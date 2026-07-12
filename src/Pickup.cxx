#include "Pickup.hxx"

#include <cmath>

Pickup::Pickup(Vector3 start, ReagentType type, int amount)
	: Reagent(type, amount) {
	position = start;
	bobTime = start.x * 0.17f + start.z * 0.11f;
}

void Pickup::Update(float dt) {
	bobTime += dt * 2.5f;
	position.y = 0.65f + std::sin(bobTime) * 0.12f;
}

void Pickup::DrawWorld() const {
	Reagent::DrawWorld();
}
