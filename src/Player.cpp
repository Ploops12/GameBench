#include "Player.hxx"

#include <raymath.h>

#include <algorithm>
#include <cmath>

namespace {
constexpr float PI_F = 3.14159265f;
Vector3 directionFromAngles(float yaw, float pitch) {
	float y = yaw * PI_F / 180.0f;
	float p = pitch * PI_F / 180.0f;
	return Vector3Normalize({cosf(y) * cosf(p), sinf(p), sinf(y) * cosf(p)});
}
}

Player::Player() {
	position = {0.0f, 1.75f, 12.0f};
	camera.position = position;
	camera.target = Vector3Add(position, {0.0f, 0.0f, -1.0f});
	camera.up = {0.0f, 1.0f, 0.0f};
	camera.fovy = 72.0f;
	camera.projection = CAMERA_PERSPECTIVE;
}

void Player::update(const InputHandler::InputState& input, float dt, bool rummaging) {
	previousPosition = position;
	yaw += input.lookInput.x * 0.12f;
	pitch = std::clamp(pitch - input.lookInput.y * 0.12f, -80.0f, 80.0f);
	Vector3 forward = directionFromAngles(yaw, 0.0f);
	Vector3 right = {forward.z, 0.0f, -forward.x};
	Vector3 wish = Vector3Add(Vector3Scale(forward, input.moveInput.x), Vector3Scale(right, input.moveInput.y));
	float speed = IsKeyDown(KEY_LEFT_SHIFT) && !rummaging ? 9.0f : 5.5f;
	if (rummaging) speed *= 0.45f;
	if (Vector3Length(wish) > 0.0f) wish = Vector3Scale(Vector3Normalize(wish), speed);
	velocity.x = wish.x;
	velocity.z = wish.z;
	if (position.y <= 1.75f && input.moveInput.z > 0.0f && !rummaging) velocity.y = 7.0f;
	velocity.y -= 18.0f * dt;
	position = Vector3Add(position, Vector3Scale(velocity, dt));
	if (position.y < 1.75f) { position.y = 1.75f; velocity.y = 0.0f; }
	if (!wardActive) ward = std::min(100.0f, ward + 10.0f * dt);
	else { ward -= 20.0f * dt; if (ward <= 0.0f) { ward = 0.0f; wardActive = false; } }
	camera.position = position;
	camera.target = Vector3Add(position, directionFromAngles(yaw, pitch));
}

void Player::constrain(float halfExtent) { position.x = std::clamp(position.x, -halfExtent, halfExtent); position.z = std::clamp(position.z, -halfExtent, halfExtent); }
void Player::resolveCollisions(const std::vector<BoundingBox>& obstacles) {
	BoundingBox body{{position.x - radius, 0.15f, position.z - radius}, {position.x + radius, 1.75f, position.z + radius}};
	for (const BoundingBox& obstacle : obstacles) {
		if (!CheckCollisionBoxes(body, obstacle)) continue;
		Vector3 shift = Vector3Subtract(previousPosition, position);
		position = previousPosition;
		camera.position = position;
		camera.target = Vector3Add(camera.target, shift);
		break;
	}
}
void Player::damage(float amount) {
	if (wardActive && ward > 0.0f) ward = std::clamp(ward - amount * 1.7f, 0.0f, 100.0f);
	else health = std::clamp(health - amount, 0.0f, 100.0f);
}
void Player::restoreWard(float amount) { ward = std::clamp(ward + amount, 0.0f, 100.0f); }
void Player::toggleWard() { if (ward > 8.0f) wardActive = !wardActive; }
