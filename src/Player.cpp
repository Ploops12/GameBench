#include <algorithm>
#include <cmath>
#include "CoatMenu.h"
#include "Player.hxx"
#include <raymath.h>

namespace {
constexpr float kBaseMoveSpeed = 5.2f;
constexpr float kSprintMultiplier = 1.45f;
constexpr float kJumpSpeed = 6.2f;
constexpr float kGravity = 18.0f;
constexpr float kMouseSensitivity = 0.0024f;
constexpr float kGroundY = 0.0f;
}

Player::Player()
	: Actor({0.0f, 0.0f, 0.0f}, 0.35f, 100.0f, kBaseMoveSpeed),
		camera {},
		inventory {},
		yaw(0.0f),
		pitch(0.0f),
		eyeHeight(1.55f),
		verticalVelocity(0.0f),
		ward(60.0f),
		maxWard(60.0f),
		wardCooldown(0.0f),
		castCooldown(0.0f),
		castFlash(0.0f),
		onGround(true),
		rummaging(false),
		warding(false),
		sprinting(false) {
	inventory.fill(0);
	position = {0.0f, 0.0f, 6.0f};

	camera.position = {position.x, position.y + eyeHeight, position.z};
	camera.target = {position.x, position.y + eyeHeight, position.z - 1.0f};
	camera.up = {0.0f, 1.0f, 0.0f};
	camera.fovy = 75.0f;
	camera.projection = CAMERA_PERSPECTIVE;
}

void Player::update(float dt, const InputHandler::InputState& input, const CoatMenu& coatMenu) {
	castCooldown = std::max(0.0f, castCooldown - dt);
	castFlash = std::max(0.0f, castFlash - dt * 2.5f);

	rummaging = coatMenu.isVisible();
	updateWard(dt, input);
	updateMovement(dt, input, coatMenu);
	syncCamera();
}

void Player::syncCamera() {
	camera.position = {position.x, position.y + eyeHeight, position.z};

	Vector3 forward {
		std::cos(pitch) * std::sin(yaw),
		std::sin(pitch),
		std::cos(pitch) * std::cos(yaw)
	};
	camera.target = Vector3Add(camera.position, forward);
}

void Player::applyWorldBounds(float minX, float maxX, float minZ, float maxZ) {
	position.x = std::clamp(position.x, minX, maxX);
	position.z = std::clamp(position.z, minZ, maxZ);
}

const Camera3D& Player::getCamera() const {
	return camera;
}

Vector3 Player::getForward() const {
	Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
	forward.y = 0.0f;
	if (Vector3LengthSqr(forward) < 0.001f) {
		return {0.0f, 0.0f, -1.0f};
	}
	return Vector3Normalize(forward);
}

Vector3 Player::getRight() const {
	Vector3 right = Vector3CrossProduct(getForward(), {0.0f, 1.0f, 0.0f});
	if (Vector3LengthSqr(right) < 0.001f) {
		return {1.0f, 0.0f, 0.0f};
	}
	return Vector3Normalize(right);
}

float Player::getEyeHeight() const {
	return eyeHeight;
}

float Player::getFeetY() const {
	return position.y;
}

bool Player::isOnGround() const {
	return onGround;
}

bool Player::isRummaging() const {
	return rummaging;
}

bool Player::isWarding() const {
	return warding;
}

bool Player::isSprinting() const {
	return sprinting;
}

float Player::getWard() const {
	return ward;
}

float Player::getMaxWard() const {
	return maxWard;
}

float Player::getCastCooldown() const {
	return castCooldown;
}

float Player::getCastFlash() const {
	return castFlash;
}

float Player::getRummageDamageScale() const {
	return rummaging ? 1.25f : 1.0f;
}

float Player::getWardCooldown() const {
	return wardCooldown;
}

void Player::addReagent(ReagentType type, int amount) {
	inventory[getReagentIndex(type)] += amount;
}

const ReagentInventory& Player::getInventory() const {
	return inventory;
}

bool Player::consumeReagents(const SpellRecipe& recipe) {
	for (const ReagentInfo& info : getReagentInfos()) {
		const int need = recipe.counts[getReagentIndex(info.type)];
		if (need > inventory[getReagentIndex(info.type)]) {
			return false;
		}
	}

	for (const ReagentInfo& info : getReagentInfos()) {
		inventory[getReagentIndex(info.type)] -= recipe.counts[getReagentIndex(info.type)];
	}
	return true;
}

void Player::spendWard(float amount) {
	ward = std::max(0.0f, ward - amount);
	if (ward <= 0.0f) {
		warding = false;
		wardCooldown = 1.1f;
	}
}

void Player::takeHit(float amount) {
	float scaledDamage = amount * getRummageDamageScale();
	if (warding && ward > 0.0f) {
		float absorbed = std::min(ward, scaledDamage);
		ward -= absorbed;
		scaledDamage -= absorbed;
		castFlash = 0.9f;
		if (ward <= 0.0f) {
			warding = false;
			wardCooldown = 1.1f;
		}
	}

	if (scaledDamage > 0.0f) {
		Actor::takeDamage(scaledDamage);
		castFlash = 1.0f;
	}
}

void Player::gainWard(float amount) {
	ward = std::min(maxWard, ward + amount);
}

void Player::triggerCastFlash() {
	castCooldown = 0.18f;
	castFlash = 1.0f;
}

void Player::updateMovement(float dt, const InputHandler::InputState& input, const CoatMenu& coatMenu) {
	yaw += input.lookInput.x * kMouseSensitivity;
	pitch -= input.lookInput.y * kMouseSensitivity;
	pitch = std::clamp(pitch, -1.2f, 1.2f);

	Vector2 move = input.moveInput;
	if (Vector2Length(move) > 1.0f) {
		move = Vector2Normalize(move);
	}

	sprinting = input.sprintDown && !coatMenu.isVisible() && move.y > 0.1f;
	float speed = moveSpeed * (sprinting ? kSprintMultiplier : 1.0f) * coatMenu.getMovementScale();

	Vector3 wish = Vector3Add(
		Vector3Scale(getRight(), move.x),
		Vector3Scale(getForward(), move.y)
	);

	if (Vector3LengthSqr(wish) > 0.001f) {
		wish = Vector3Scale(Vector3Normalize(wish), speed);
	}

	position.x += wish.x * dt;
	position.z += wish.z * dt;

	if (input.jumpPressed && onGround && !coatMenu.isVisible()) {
		verticalVelocity = kJumpSpeed;
		onGround = false;
	}

	verticalVelocity -= kGravity * dt;
	position.y += verticalVelocity * dt;

	if (position.y <= kGroundY) {
		position.y = kGroundY;
		verticalVelocity = 0.0f;
		onGround = true;
	}
}

void Player::updateWard(float dt, const InputHandler::InputState& input) {
	wardCooldown = std::max(0.0f, wardCooldown - dt);

	if (input.wardDown && ward > 0.0f && wardCooldown <= 0.0f) {
		warding = true;
		spendWard(dt * 8.5f);
	} else {
		warding = false;
		if (wardCooldown <= 0.0f) {
			ward = std::min(maxWard, ward + dt * 11.0f);
		}
	}
}
