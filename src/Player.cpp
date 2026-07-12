#include "Player.hxx"

#include <algorithm>
#include <cmath>

#include <raymath.h>

namespace {
constexpr float CAMERA_HEIGHT = 1.65f;
constexpr float PLAYER_RADIUS = 0.34f;
constexpr float MOVE_SPEED = 5.4f;
constexpr float SPRINT_SPEED = 8.4f;
constexpr float GRAVITY = 23.0f;
constexpr float JUMP_SPEED = 8.3f;
}

Player::Player() : Actor({0.0f, 0.0f, 10.0f}, 100.0f) {
	collisionRadius = PLAYER_RADIUS;
	camera.fovy = 75.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	updateCamera();
}

void Player::update(const InputHandler::InputState& input, float deltaTime, const std::vector<BoundingBox>& obstacles) {
	yaw += input.lookInput.x * 0.0025f;
	pitch = std::clamp(pitch - input.lookInput.y * 0.0025f, -1.25f, 1.25f);
	rummaging = input.rummageHeld;
	sprinting = input.sprintHeld && !rummaging && input.moveInput.x > 0.0f;

	if (onGround && input.moveInput.z > 0.0f && !rummaging) {
		verticalVelocity = JUMP_SPEED;
		onGround = false;
	}
	verticalVelocity -= GRAVITY * deltaTime;
	position.y += verticalVelocity * deltaTime;
	if (position.y <= 0.0f) {
		position.y = 0.0f;
		verticalVelocity = 0.0f;
		onGround = true;
	}

	Vector3 forward = getForward();
	forward.y = 0.0f;
	forward = Vector3Normalize(forward);
	Vector3 right {forward.z, 0.0f, -forward.x};
	Vector3 move = Vector3Add(Vector3Scale(forward, input.moveInput.x), Vector3Scale(right, input.moveInput.y));
	if (Vector3Length(move) > 0.01f) move = Vector3Normalize(move);
	float speed = sprinting ? SPRINT_SPEED : MOVE_SPEED;
	if (rummaging) speed *= 0.45f;
	move = Vector3Scale(move, speed * deltaTime);

	Vector3 horizontal = position;
	horizontal.x += move.x;
	if (!collides(horizontal, obstacles)) position.x = horizontal.x;
	horizontal = position;
	horizontal.z += move.z;
	if (!collides(horizontal, obstacles)) position.z = horizontal.z;
	position.x = std::clamp(position.x, -18.8f, 18.8f);
	position.z = std::clamp(position.z, -18.8f, 18.8f);

	wardActive = input.wardHeld && ward > 0.0f;
	if (wardActive) {
		ward = std::max(0.0f, ward - 18.0f * deltaTime);
		wardDelay = 1.15f;
	} else {
		wardDelay = std::max(0.0f, wardDelay - deltaTime);
		if (wardDelay <= 0.0f) ward = std::min(100.0f, ward + 13.0f * deltaTime);
	}
	wardFlash = std::max(0.0f, wardFlash - deltaTime * 2.5f);
	updateCamera();
}

const Camera3D& Player::getCamera() const {
	return camera;
}

Vector3 Player::getForward() const {
	return {std::cos(pitch) * std::cos(yaw), std::sin(pitch), std::cos(pitch) * std::sin(yaw)};
}

bool Player::isRummaging() const { return rummaging; }
bool Player::isSprinting() const { return sprinting; }
bool Player::isWardActive() const { return wardActive; }
float Player::getWard() const { return ward; }
float Player::getMaxWard() const { return 100.0f; }
float Player::getWardFlash() const { return wardFlash; }

void Player::addWard(float amount) {
	ward = std::min(100.0f, ward + amount);
	wardFlash = 1.0f;
}

void Player::heal(float amount) {
	health = std::min(maxHealth, health + amount);
}

void Player::takeDamage(float amount) {
	wardDelay = 1.15f;
	if (wardActive && ward > 0.0f) {
		float absorbed = std::min(ward, amount * 1.25f);
		ward -= absorbed;
		amount -= absorbed / 1.25f;
		wardFlash = 1.0f;
	}
	Actor::takeDamage(amount);
}

std::array<int, REAGENT_COUNT>& Player::getInventory() { return inventory; }
const std::array<int, REAGENT_COUNT>& Player::getInventory() const { return inventory; }

void Player::updateCamera() {
	camera.position = {position.x, position.y + CAMERA_HEIGHT, position.z};
	camera.target = Vector3Add(camera.position, getForward());
	camera.up = {0.0f, 1.0f, 0.0f};
}

bool Player::collides(Vector3 candidate, const std::vector<BoundingBox>& obstacles) const {
	BoundingBox body {{candidate.x - PLAYER_RADIUS, candidate.y, candidate.z - PLAYER_RADIUS},
		{candidate.x + PLAYER_RADIUS, candidate.y + 1.6f, candidate.z + PLAYER_RADIUS}};
	for (const BoundingBox& obstacle : obstacles) {
		if (CheckCollisionBoxes(body, obstacle)) return true;
	}
	return false;
}
