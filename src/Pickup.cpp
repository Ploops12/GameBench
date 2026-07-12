#include "Pickup.hxx"

#include <raymath.h>

#include <cmath>

Pickup::Pickup(Vector3 position, ReagentType type, int quantity)
	: Object(position, 0.28f), type_(type), quantity_(quantity), baseHeight_(position.y + 0.35f) {
	position_.y = baseHeight_;
	setCollisionEnabled(false);
}

void Pickup::update(float dt) {
	age_ += dt;
	position_.y = baseHeight_ + std::sin(age_ * 2.2f) * 0.12f;
}

bool Pickup::collectIfNear(Vector3 playerPosition, float range) {
	if (!isActive()) {
		return false;
	}
	playerPosition.y = position_.y;
	if (Vector3Distance(playerPosition, position_) <= range) {
		setActive(false);
		return true;
	}
	return false;
}

void Pickup::drawBillboardHud(const Camera3D& camera) const {
	if (!isActive()) {
		return;
	}
	const Vector3 toPickup = Vector3Subtract(position_, camera.position);
	const Vector3 view = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
	if (Vector3DotProduct(toPickup, view) <= 0.0f || Vector3Length(toPickup) > 7.0f) {
		return;
	}
	const Vector2 screen = GetWorldToScreen({position_.x, position_.y + 0.5f, position_.z}, camera);
	const auto& definition = GetReagentDefinition(type_);
	const char* text = TextFormat("%s +%i", definition.shortName.data(), quantity_);
	const int width = MeasureText(text, 14);
	DrawRectangle(static_cast<int>(screen.x - width / 2 - 4), static_cast<int>(screen.y - 2), width + 8, 18, {7, 10, 14, 205});
	DrawText(text, static_cast<int>(screen.x - width / 2), static_cast<int>(screen.y), 14, definition.color);
}

ReagentType Pickup::type() const {
	return type_;
}

int Pickup::quantity() const {
	return quantity_;
}

void Pickup::onDraw(const Camera3D&) const {
	const auto& definition = GetReagentDefinition(type_);
	const float pulse = 0.27f + std::sin(age_ * 3.0f) * 0.035f;
	DrawSphereWires(position_, pulse, 5, 8, definition.color);
	DrawLine3D({position_.x, position_.y - 0.35f, position_.z}, {position_.x, position_.y + 0.35f, position_.z}, ColorAlpha(definition.color, 0.65f));
	DrawLine3D({position_.x - 0.20f, position_.y, position_.z}, {position_.x + 0.20f, position_.y, position_.z}, definition.color);
}
