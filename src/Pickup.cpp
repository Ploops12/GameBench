#include "Pickup.hxx"
#include <cmath>

Pickup::Pickup(Vector3 position, ReagentType type, int quantity) :
	Object(position, 0.65f), type(type), quantity(quantity) {}

void Pickup::update(float dt) {
	bobTime += dt;
}

void Pickup::draw3D() const {
	Vector3 p = position;
	p.y += 0.18f + std::sin(bobTime * 3.0f) * 0.12f;
	const Color color = reagentColor(type);
	DrawSphereWires(p, 0.23f, 5, 6, color);
	DrawLine3D(Vector3{p.x - 0.28f, p.y, p.z}, Vector3{p.x + 0.28f, p.y, p.z}, color);
	DrawLine3D(Vector3{p.x, p.y - 0.28f, p.z}, Vector3{p.x, p.y + 0.28f, p.z}, color);
}
