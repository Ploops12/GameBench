#include "Player.hxx"

#include <raymath.h>
#include <algorithm>
#include <cmath>

namespace {
	float DegreesToRadians(float degrees) { return degrees * PI / 180.0f; }
}

Player::Player() {
	position = {0.0f, 1.65f, 7.5f};
	radius = 0.35f;
	camera.position = position;
	camera.fovy = 72.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	camera.up = {0.0f, 1.0f, 0.0f};
	camera.target = {0.0f, 1.65f, 6.5f};
}

Vector3 Player::GetForward() const {
	const float yawRadians = DegreesToRadians(yaw);
	const float pitchRadians = DegreesToRadians(pitch);
	return Vector3Normalize({std::cos(yawRadians) * std::cos(pitchRadians), std::sin(pitchRadians), std::sin(yawRadians) * std::cos(pitchRadians)});
}

void Player::SetPosition(Vector3 value) {
	position = value;
	camera.position = position;
}

void Player::Update(const InputHandler::InputState& input, float dt, bool rummaging, const std::vector<BoundingBox>& obstacles) {
	const float lookSpeed = 0.075f;
	yaw += input.lookInput.x * lookSpeed;
	pitch = std::clamp(pitch - input.lookInput.y * lookSpeed, -78.0f, 78.0f);

	wardCooldown = std::fmax(0.0f, wardCooldown - dt);
	if (wardActive) {
		wardTimer -= dt;
		if (wardTimer <= 0.0f) wardActive = false;
	} else {
		ward = std::fmin(maxWard, ward + dt * 4.5f);
	}

	Vector3 forward = GetForward();
	forward.y = 0.0f;
	forward = Vector3Normalize(forward);
	const Vector3 right{-forward.z, 0.0f, forward.x};
	Vector3 movement = Vector3Add(Vector3Scale(forward, input.moveInput.x), Vector3Scale(right, input.moveInput.y));
	if (Vector3Length(movement) > 1.0f) movement = Vector3Normalize(movement);
	const float speed = rummaging ? 2.0f : (input.sprintHeld ? 7.0f : 4.3f);
	Vector3 candidate = Vector3Add(position, Vector3Scale(movement, speed * dt));
	for (const BoundingBox& obstacle : obstacles) {
		if (CheckCollisionBoxSphere(obstacle, candidate, radius)) {
			candidate.x = position.x;
			if (!CheckCollisionBoxSphere(obstacle, candidate, radius)) break;
			candidate.z = position.z;
		}
	}
	position.x = std::clamp(candidate.x, -13.0f, 13.0f);
	position.z = std::clamp(candidate.z, -17.0f, 11.0f);

	if (input.jumpPressed && onGround && !rummaging) {
		verticalVelocity = 6.0f;
		onGround = false;
	}
	verticalVelocity -= 15.0f * dt;
	position.y += verticalVelocity * dt;
	if (position.y <= 1.65f) {
		position.y = 1.65f;
		verticalVelocity = 0.0f;
		onGround = true;
	}

	camera.position = position;
	camera.target = Vector3Add(position, GetForward());
}

void Player::ActivateWard() {
	if (wardActive || wardCooldown > 0.0f || ward < 18.0f) return;
	ward -= 18.0f;
	wardActive = true;
	wardTimer = 0.85f;
	wardCooldown = 2.6f;
}

float Player::TakeDamage(float amount) {
	if (wardActive) return 0.0f;
	const float damage = std::fmax(0.0f, amount);
	health = std::fmax(0.0f, health - damage);
	return damage;
}

void Player::Heal(float amount) {
	health = std::fmin(maxHealth, health + std::fmax(0.0f, amount));
}

void Player::GainWard(float amount) {
	ward = std::fmin(maxWard, ward + std::fmax(0.0f, amount));
}

void Player::DrawWardOverlay() const {
	if (!wardActive) return;
	const int centerX = GetScreenWidth() / 2;
	const int centerY = GetScreenHeight() / 2;
	DrawCircleLines(centerX, centerY, 96.0f, Fade(SKYBLUE, 0.75f));
	DrawCircleLines(centerX, centerY, 104.0f, Fade(WHITE, 0.45f));
	DrawText("WARD", centerX - 24, centerY + 112, 16, SKYBLUE);
}
