#include "Player.hxx"

#include <raymath.h>
#include <algorithm>
#include <cmath>

Player::Player() {
	reset();
}

void Player::reset() {
	position = {0.0f, 0.0f, 8.0f};
	velocity = {};
	health = maxHealth = 100.0f;
	ward = maxWard = 100.0f;
	inventory.fill(0);
	yaw = PI;
	pitch = 0.0f;
	grounded = true;
	camera = {};
	camera.up = {0.0f, 1.0f, 0.0f};
	camera.fovy = 78.0f;
	camera.projection = CAMERA_PERSPECTIVE;
}

Vector3 Player::forward() const {
	return Vector3Normalize({std::sin(yaw) * std::cos(pitch), std::sin(pitch), std::cos(yaw) * std::cos(pitch)});
}

Vector3 Player::eyePosition() const {
	return {position.x, position.y + 1.65f, position.z};
}

void Player::update(float dt, const InputHandler::InputState& state, bool rummaging) {
	yaw -= state.lookInput.x * 0.0024f;
	pitch = std::clamp(pitch - state.lookInput.y * 0.0024f, -1.35f, 1.35f);
	Vector3 flatForward = Vector3Normalize({std::sin(yaw), 0.0f, std::cos(yaw)});
	Vector3 right = {flatForward.z, 0.0f, -flatForward.x};
	Vector3 wish = Vector3Add(Vector3Scale(flatForward, state.moveInput.x), Vector3Scale(right, state.moveInput.y));
	if (Vector3LengthSqr(wish) > 1.0f) wish = Vector3Normalize(wish);
	const bool sprint = IsKeyDown(KEY_LEFT_SHIFT) && !rummaging;
	const float speed = rummaging ? 3.0f : (sprint ? 9.5f : 6.5f);
	velocity.x = wish.x * speed;
	velocity.z = wish.z * speed;
	if (state.jumpPressed && grounded && !rummaging) {
		velocity.y = 7.2f;
		grounded = false;
	}
	velocity.y -= 19.0f * dt;
	position = Vector3Add(position, Vector3Scale(velocity, dt));
	if (position.y <= 0.0f) {
		position.y = 0.0f;
		velocity.y = 0.0f;
		grounded = true;
	}
	position.x = std::clamp(position.x, -18.2f, 18.2f);
	position.z = std::clamp(position.z, -18.2f, 18.2f);
	wardActive = IsKeyDown(KEY_Q) && ward > 0.0f;
	if (wardActive) ward = std::max(0.0f, ward - 24.0f * dt);
	else if (wardRegenDelay <= 0.0f) ward = std::min(maxWard, ward + 12.0f * dt);
	wardRegenDelay = std::max(0.0f, wardRegenDelay - dt);
	camera.position = eyePosition();
	camera.target = Vector3Add(camera.position, forward());
}

void Player::damage(float amount) {
	wardRegenDelay = 2.0f;
	if (wardActive && ward > 0.0f) {
		const float absorbed = std::min(ward, amount * 1.8f);
		ward -= absorbed;
		amount -= absorbed / 1.8f;
	}
	health = std::max(0.0f, health - amount);
}
