#include "Player.hxx"

#include <algorithm>
#include <cmath>

#include <raymath.h>

namespace {

void drawHand(Vector2 wrist, bool mirrored, Color color, float lift) {
	const float sign = mirrored ? -1.0f : 1.0f;
	const Vector2 palm {wrist.x + sign * 42.0f, wrist.y - 76.0f - lift};
	const Vector2 knuckle {wrist.x + sign * 70.0f, wrist.y - 133.0f - lift};
	const Color outline = RAYWHITE;

	DrawLineEx(wrist, palm, 8.0f, Fade(color, 0.25f));
	DrawLineEx(wrist, palm, 2.0f, outline);
	DrawLineEx(palm, knuckle, 3.0f, color);
	DrawCircleLines(static_cast<int>(palm.x), static_cast<int>(palm.y), 19.0f, outline);

	for (int finger = -2; finger <= 2; ++finger) {
		const float offset = static_cast<float>(finger) * 12.0f;
		const Vector2 fingerBase {knuckle.x + sign * offset * 0.38f, knuckle.y + std::abs(offset) * 0.18f};
		const Vector2 fingerTip {knuckle.x + sign * (24.0f + offset * 0.54f), knuckle.y - 40.0f + std::abs(offset) * 0.42f};
		DrawLineEx(fingerBase, fingerTip, 2.2f, outline);
		DrawCircleLines(static_cast<int>(fingerTip.x), static_cast<int>(fingerTip.y), 3.0f, color);
	}

	DrawLineEx({palm.x, palm.y + 13.0f}, {palm.x + sign * 14.0f, palm.y - 5.0f}, 2.0f, color);
	DrawLineEx({palm.x, palm.y + 13.0f}, {palm.x + sign * 18.0f, palm.y + 18.0f}, 2.0f, color);
}

}

Player::Player()
	: Actor({0.0f, 0.0f, 8.0f}, 0.35f, 100.0f) {
	camera.up = {0.0f, 1.0f, 0.0f};
	camera.fovy = 76.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	updateCamera();
}

void Player::update(const InputHandler::InputState& input, float deltaTime,
	const std::vector<BoundingBox>& colliders, bool rummaging) {
	if (!isAlive()) {
		return;
	}

	yaw += input.lookInput.x * 0.0026f;
	pitch = std::clamp(pitch - input.lookInput.y * 0.0026f, -1.25f, 1.25f);
	castCooldown = std::max(0.0f, castCooldown - deltaTime);
	damageFlash = std::max(0.0f, damageFlash - deltaTime);

	wardBreakCooldown = std::max(0.0f, wardBreakCooldown - deltaTime);
	wardActive = input.wardHeld && ward > 0.0f && wardBreakCooldown <= 0.0f;
	if (wardActive) {
		ward = std::max(0.0f, ward - 24.0f * deltaTime);
		if (ward <= 0.0f) {
			wardActive = false;
			wardBreakCooldown = 1.2f;
		}
	} else if (wardBreakCooldown <= 0.0f) {
		ward = std::min(maxWard, ward + 14.0f * deltaTime);
	}

	if (input.jumpPressed && grounded && !rummaging) {
		verticalVelocity = 6.4f;
		grounded = false;
	}

	const Vector3 forwardFlat {std::cos(yaw), 0.0f, std::sin(yaw)};
	const Vector3 rightFlat {-forwardFlat.z, 0.0f, forwardFlat.x};
	Vector3 move = Vector3Add(Vector3Scale(forwardFlat, input.moveInput.x), Vector3Scale(rightFlat, input.moveInput.y));
	if (Vector3Length(move) > 0.01f) {
		move = Vector3Normalize(move);
	}

	float moveSpeed = 6.1f;
	if (input.sprintHeld && !rummaging && grounded) {
		moveSpeed = 9.3f;
	}
	if (rummaging) {
		moveSpeed *= 0.46f;
	}

	Vector3 candidate = position;
	candidate.x += move.x * moveSpeed * deltaTime;
	if (!collidesWithWorld(candidate, colliders)) {
		position.x = candidate.x;
	}
	candidate = position;
	candidate.z += move.z * moveSpeed * deltaTime;
	if (!collidesWithWorld(candidate, colliders)) {
		position.z = candidate.z;
	}
	position.x = std::clamp(position.x, -23.0f, 23.0f);
	position.z = std::clamp(position.z, -23.0f, 23.0f);

	verticalVelocity -= 18.0f * deltaTime;
	position.y += verticalVelocity * deltaTime;
	if (position.y <= 0.0f) {
		position.y = 0.0f;
		verticalVelocity = 0.0f;
		grounded = true;
	}

	updateCamera();
}

void Player::takeDamage(float amount) {
	float remainingDamage = std::max(0.0f, amount);
	if (wardActive && ward > 0.0f) {
		const float wardCost = std::min(ward, remainingDamage * 1.35f);
		ward -= wardCost;
		remainingDamage -= wardCost / 1.35f;
		if (ward <= 0.0f) {
			wardActive = false;
			wardBreakCooldown = 1.2f;
		}
	}

	Actor::takeDamage(remainingDamage);
	damageFlash = 0.22f;
}

const Camera3D& Player::getCamera() const {
	return camera;
}

Vector3 Player::getEyePosition() const {
	return camera.position;
}

Vector3 Player::getForward() const {
	return Vector3Normalize(Vector3Subtract(camera.target, camera.position));
}

Vector3 Player::getRight() const {
	const Vector3 forward = getForward();
	return Vector3Normalize(Vector3CrossProduct(forward, camera.up));
}

bool Player::canCast() const {
	return castCooldown <= 0.0f && isAlive();
}

void Player::beginCast(float cooldown) {
	castCooldown = std::max(0.05f, cooldown);
}

float Player::getCastCooldown() const {
	return castCooldown;
}

float Player::getWard() const {
	return ward;
}

float Player::getMaxWard() const {
	return maxWard;
}

bool Player::isWardActive() const {
	return wardActive;
}

float Player::getDamageFlash() const {
	return damageFlash;
}

void Player::addWard(float amount) {
	ward = std::min(maxWard, ward + std::max(0.0f, amount));
}

void Player::addReagent(ReagentType type, int amount) {
	inventory[reagentIndex(type)] += std::max(0, amount);
}

const std::array<int, ReagentCount>& Player::getInventory() const {
	return inventory;
}

bool Player::consumeReagents(const CurseVector& spell) {
	std::array<int, ReagentCount> costs {};
	for (int index = 0; index < spell.getReagentCount(); ++index) {
		const ReagentType type = spell.getReagents()[static_cast<std::size_t>(index)];
		++costs[reagentIndex(type)];
	}

	for (std::size_t index = 0; index < ReagentCount; ++index) {
		if (costs[index] > inventory[index]) {
			return false;
		}
	}
	for (std::size_t index = 0; index < ReagentCount; ++index) {
		inventory[index] -= costs[index];
	}
	return true;
}

void Player::drawHands(float time, const SpellStats& spell, bool rummaging) const {
	const int screenWidth = GetScreenWidth();
	const int screenHeight = GetScreenHeight();
	const float sway = std::sin(time * 5.0f) * 4.0f;
	Color handColor = spell.color;
	if (wardActive) {
		handColor = {150, 237, 255, 255};
	}
	if (rummaging) {
		handColor = {244, 211, 137, 255};
	}

	drawHand({screenWidth * 0.20f, static_cast<float>(screenHeight) + 55.0f}, false, handColor, sway);
	drawHand({screenWidth * 0.80f, static_cast<float>(screenHeight) + 55.0f}, true, handColor, -sway);

	if (wardActive) {
		DrawCircleLines(screenWidth / 2, screenHeight / 2, 54.0f + std::sin(time * 9.0f) * 3.0f, handColor);
		DrawCircleLines(screenWidth / 2, screenHeight / 2, 72.0f, Fade(handColor, 0.6f));
	}
}

bool Player::collidesWithWorld(Vector3 candidatePosition, const std::vector<BoundingBox>& colliders) const {
	const float radius = collisionRadius;
	for (const BoundingBox& collider : colliders) {
		if (candidatePosition.x + radius > collider.min.x && candidatePosition.x - radius < collider.max.x &&
			candidatePosition.z + radius > collider.min.z && candidatePosition.z - radius < collider.max.z &&
			candidatePosition.y < collider.max.y) {
			return true;
		}
	}
	return false;
}

void Player::updateCamera() {
	const Vector3 forward {
		std::cos(pitch) * std::cos(yaw),
		std::sin(pitch),
		std::cos(pitch) * std::sin(yaw)
	};
	camera.position = {position.x, position.y + 1.58f, position.z};
	camera.target = Vector3Add(camera.position, forward);
	camera.up = {0.0f, 1.0f, 0.0f};
	camera.fovy = 76.0f;
	camera.projection = CAMERA_PERSPECTIVE;
}
