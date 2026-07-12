#include "Player.hxx"

#include <algorithm>
#include <cmath>

#include <raymath.h>

namespace {
constexpr float PLAYER_HEIGHT = 1.76f;
constexpr float PLAYER_EYE_OFFSET = 0.66f;
constexpr float WALK_SPEED = 6.2f;
constexpr float SPRINT_SPEED = 9.1f;
constexpr float RUMMAGE_SPEED = 3.0f;
constexpr float JUMP_SPEED = 7.8f;
constexpr float GRAVITY = 21.0f;
constexpr float LOOK_SENSITIVITY = 0.00235f;
constexpr float MIN_WARD_ACTIVATION = 5.0f;
}

Player::Player()
	: Actor({0.0f, PLAYER_HEIGHT * 0.5f, 0.0f}, {0.72f, PLAYER_HEIGHT, 0.72f}, 100.0f),
	  preparedSpell(CurseVectorType::Poppet) {
	camera.position = getEyePosition();
	camera.target = {0.0f, camera.position.y, -1.0f};
	camera.up = {0.0f, 1.0f, 0.0f};
	camera.fovy = 76.0f;
	camera.projection = CAMERA_PERSPECTIVE;
}

void Player::reset(Vector3 spawnPosition) {
	position = spawnPosition;
	size = {0.72f, PLAYER_HEIGHT, 0.72f};
	velocity = {};
	health = maxHealth;
	hurtTime = 0.0f;
	snareTime = 0.0f;
	snareMultiplier = 1.0f;
	rotTime = 0.0f;
	rotDamagePerSecond = 0.0f;
	fearTime = 0.0f;
	active = true;
	grounded = spawnPosition.y <= size.y * 0.5f + 0.001f;
	yaw = 0.0f;
	pitch = 0.0f;
	inventory.fill(0);
	preparedSpell = CurseVector(CurseVectorType::Poppet);
	ward = 70.0f;
	wardActive = false;
	wardBroken = false;
	wardRegenDelay = 0.0f;
	wardLockout = 0.0f;
	wardFlash = 0.0f;
	damageFlash = 0.0f;
	hasteTime = 0.0f;
	castCooldown = 0.0f;
	bobPhase = 0.0f;
	updateCamera(0.0f, {}, 0.0f);
}

void Player::updateFromInput(float deltaTime, const InputHandler::InputState& input,
		const std::vector<BoundingBox>& solidBounds, bool rummaging) {
	Actor::update(deltaTime);
	if (!isAlive()) {
		wardActive = false;
		updateCamera(deltaTime, input.lookInput, 0.0f);
		return;
	}

	damageFlash = std::max(0.0f, damageFlash - deltaTime);
	wardFlash = std::max(0.0f, wardFlash - deltaTime);
	wardLockout = std::max(0.0f, wardLockout - deltaTime);
	wardRegenDelay = std::max(0.0f, wardRegenDelay - deltaTime);
	hasteTime = std::max(0.0f, hasteTime - deltaTime);
	tickCastCooldown(deltaTime);

	yaw += input.lookInput.x * LOOK_SENSITIVITY;
	pitch -= input.lookInput.y * LOOK_SENSITIVITY;
	pitch = std::clamp(pitch, -1.42f, 1.42f);

	const Vector3 forward = getFlatForward();
	const Vector3 right = {std::cos(yaw), 0.0f, std::sin(yaw)};
	Vector3 move = Vector3Add(Vector3Scale(forward, input.moveInput.x),
		Vector3Scale(right, input.moveInput.y));
	float moveAmount = Vector3Length(move);
	if (moveAmount > 1.0f) {
		move = Vector3Scale(move, 1.0f / moveAmount);
		moveAmount = 1.0f;
	}

	float speed = WALK_SPEED;
	if (rummaging) {
		speed = RUMMAGE_SPEED;
	} else if (input.sprintHeld) {
		speed = SPRINT_SPEED;
	}
	if (hasteTime > 0.0f) {
		speed *= 1.25f;
	}
	speed *= getMovementMultiplier();

	const Vector3 desired = Vector3Scale(move, speed);
	const float response = 1.0f - std::exp(-(moveAmount > 0.0f ? 18.0f : 12.0f) * deltaTime);
	velocity.x += (desired.x - velocity.x) * response;
	velocity.z += (desired.z - velocity.z) * response;

	if (input.jumpPressed && grounded) {
		velocity.y = JUMP_SPEED;
		grounded = false;
	}
	velocity.y -= GRAVITY * deltaTime;

	Vector3 candidate = position;
	candidate.x += velocity.x * deltaTime;
	if (!collidesAt(candidate, solidBounds)) {
		position.x = candidate.x;
	} else {
		velocity.x = 0.0f;
	}

	candidate = position;
	candidate.z += velocity.z * deltaTime;
	if (!collidesAt(candidate, solidBounds)) {
		position.z = candidate.z;
	} else {
		velocity.z = 0.0f;
	}

	position.y += velocity.y * deltaTime;
	const float groundHeight = size.y * 0.5f;
	if (position.y <= groundHeight) {
		position.y = groundHeight;
		velocity.y = 0.0f;
		grounded = true;
	}

	setWardRequested(input.wardHeld, deltaTime);
	if (!wardActive && wardLockout <= 0.0f && wardRegenDelay <= 0.0f) {
		ward = std::min(maxWard, ward + 13.0f * deltaTime);
	}

	if (grounded && moveAmount > 0.05f) {
		bobPhase += deltaTime * (rummaging ? 7.0f : (input.sprintHeld ? 13.0f : 10.0f));
	}
	updateCamera(deltaTime, {}, moveAmount);
}

float Player::takeDamage(float damage) {
	if (!isAlive() || damage <= 0.0f) {
		return 0.0f;
	}

	float healthDamage = damage;
	if (wardActive && ward > 0.0f) {
		const float absorbed = std::min(ward, healthDamage);
		ward -= absorbed;
		healthDamage -= absorbed;
		wardFlash = 0.28f;
		wardRegenDelay = 2.0f;
		if (ward <= 0.0f) {
			ward = 0.0f;
			wardActive = false;
			wardBroken = true;
			wardLockout = 2.5f;
		}
	}

	if (healthDamage > 0.0f) {
		damageFlash = 0.42f;
		wardRegenDelay = 2.0f;
		return Actor::takeDamage(healthDamage);
	}
	return 0.0f;
}

void Player::draw3D(const Camera3D& cameraValue) const {
	(void)cameraValue;
}

const Camera3D& Player::getCamera() const {
	return camera;
}

Vector3 Player::getEyePosition() const {
	return {position.x, position.y + PLAYER_EYE_OFFSET, position.z};
}

Vector3 Player::getLookDirection() const {
	const float cosPitch = std::cos(pitch);
	return Vector3Normalize({std::sin(yaw) * cosPitch, std::sin(pitch), -std::cos(yaw) * cosPitch});
}

Vector3 Player::getFlatForward() const {
	return Vector3Normalize({std::sin(yaw), 0.0f, -std::cos(yaw)});
}

int Player::addReagent(ReagentType type, int amount) {
	const std::size_t index = static_cast<std::size_t>(type);
	if (index < inventory.size() && amount > 0) {
		const int accepted = std::min(amount, 99 - inventory[index]);
		inventory[index] += accepted;
		return accepted;
	}
	return 0;
}

const ReagentInventory& Player::getInventory() const {
	return inventory;
}

ReagentInventory& Player::getInventory() {
	return inventory;
}

const CurseVector& Player::getPreparedSpell() const {
	return preparedSpell;
}

void Player::setPreparedSpell(const CurseVector& spell) {
	preparedSpell = spell;
}

bool Player::consumePreparedReagents() {
	return preparedSpell.consume(inventory);
}

void Player::setWardRequested(bool requested, float deltaTime) {
	const bool wasActive = wardActive;
	if (!requested) {
		wardActive = false;
		wardBroken = false;
		if (wasActive) {
			wardRegenDelay = std::max(wardRegenDelay, 1.1f);
		}
		return;
	}

	wardActive = !wardBroken && wardLockout <= 0.0f && isAlive()
		&& (wasActive || ward >= MIN_WARD_ACTIVATION);
	if (wardActive) {
		ward = std::max(0.0f, ward - 15.0f * deltaTime);
		wardRegenDelay = std::max(wardRegenDelay, 1.1f);
		if (ward <= 0.0f) {
			wardActive = false;
			wardBroken = true;
			wardLockout = 2.5f;
		}
	} else if (wasActive) {
		wardRegenDelay = std::max(wardRegenDelay, 1.1f);
	}
}

void Player::gainWard(float amount) {
	ward = std::clamp(ward + amount, 0.0f, maxWard);
	if (amount > 0.0f) {
		wardFlash = 0.24f;
	}
}

float Player::getWard() const {
	return ward;
}

float Player::getMaxWard() const {
	return maxWard;
}

float Player::getWardRatio() const {
	return maxWard > 0.0f ? ward / maxWard : 0.0f;
}

bool Player::isWardActive() const {
	return wardActive;
}

bool Player::isWardBroken() const {
	return wardBroken;
}

float Player::getWardLockout() const {
	return wardLockout;
}

float Player::getWardFlash() const {
	return wardFlash;
}

void Player::applyHaste(float duration) {
	hasteTime = std::max(hasteTime, duration);
}

float Player::getHasteTime() const {
	return hasteTime;
}

float Player::getDamageFlash() const {
	return damageFlash;
}

void Player::tickCastCooldown(float deltaTime) {
	castCooldown = std::max(0.0f, castCooldown - deltaTime);
}

bool Player::canCast() const {
	return isAlive() && castCooldown <= 0.0f;
}

void Player::beginCastCooldown(float duration) {
	castCooldown = std::max(0.0f, duration);
}

bool Player::collidesAt(Vector3 candidate, const std::vector<BoundingBox>& solidBounds) const {
	const Vector3 half = Vector3Scale(size, 0.5f);
	const BoundingBox candidateBounds = {
		Vector3Subtract(candidate, half),
		Vector3Add(candidate, half)
	};
	for (const BoundingBox& box : solidBounds) {
		if (CheckCollisionBoxes(candidateBounds, box)) {
			return true;
		}
	}
	return false;
}

void Player::updateCamera(float deltaTime, Vector2 lookInput, float moveAmount) {
	(void)deltaTime;
	if (lookInput.x != 0.0f || lookInput.y != 0.0f) {
		yaw += lookInput.x * LOOK_SENSITIVITY;
		pitch -= lookInput.y * LOOK_SENSITIVITY;
		pitch = std::clamp(pitch, -1.42f, 1.42f);
	}
	const float bob = grounded && moveAmount > 0.05f ? std::sin(bobPhase) * 0.035f : 0.0f;
	camera.position = getEyePosition();
	camera.position.y += bob;
	camera.target = Vector3Add(camera.position, getLookDirection());
}
