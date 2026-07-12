#include "Player.hxx"
#include <algorithm>
#include <cmath>
#include <raymath.h>

namespace {
constexpr float BODY_HALF_WIDTH = 0.34f;
constexpr float BODY_HALF_HEIGHT = 0.9f;
constexpr float EYE_HEIGHT = 0.72f;

bool collides(BoundingBox box, const std::vector<BoundingBox>& obstacles) {
	for (const BoundingBox& obstacle : obstacles) {
		if (CheckCollisionBoxes(box, obstacle)) return true;
	}
	return false;
}
}

Player::Player() : Actor(Vector3{0.0f, BODY_HALF_HEIGHT, 8.0f}, BODY_HALF_WIDTH, 100.0f) {
	reset();
}

void Player::reset() {
	position = Vector3{0.0f, BODY_HALF_HEIGHT, 8.0f};
	velocity = {};
	health = maxHealth;
	active = true;
	yaw = 3.14159265f;
	pitch = 0.0f;
	grounded = true;
	inventory.fill(0);
	spell = CurseVector(BaseVector::Poppet);
	ward = 70.0f;
	wardActive = false;
	wardRegenDelay = 0.0f;
	damageFlash = 0.0f;
	castFlash = 0.0f;
	camera = Camera3D{};
	camera.up = Vector3{0.0f, 1.0f, 0.0f};
	camera.fovy = 74.0f;
	camera.projection = CAMERA_PERSPECTIVE;
}

Vector3 Player::getLookDirection() const {
	const float cp = std::cos(pitch);
	return Vector3Normalize(Vector3{std::sin(yaw) * cp, std::sin(pitch), std::cos(yaw) * cp});
}

BoundingBox Player::getBoundsAt(Vector3 value) const {
	return BoundingBox{
		Vector3{value.x - BODY_HALF_WIDTH, value.y - BODY_HALF_HEIGHT, value.z - BODY_HALF_WIDTH},
		Vector3{value.x + BODY_HALF_WIDTH, value.y + BODY_HALF_HEIGHT, value.z + BODY_HALF_WIDTH}
	};
}

void Player::update(float dt, const InputHandler::InputState& input,
	const std::vector<BoundingBox>& obstacles, bool rummaging) {
	yaw -= input.lookInput.x * 0.0022f;
	pitch = std::clamp(pitch - input.lookInput.y * 0.0022f, -1.42f, 1.42f);

	Vector3 forward{std::sin(yaw), 0.0f, std::cos(yaw)};
	Vector3 right{forward.z, 0.0f, -forward.x};
	Vector3 wish = Vector3Add(Vector3Scale(forward, input.moveInput.x), Vector3Scale(right, input.moveInput.y));
	if (Vector3LengthSqr(wish) > 1.0f) wish = Vector3Normalize(wish);
	float speed = 5.6f;
	if (input.sprintDown && !rummaging) speed = 8.2f;
	if (rummaging) speed *= 0.43f;
	velocity.x = wish.x * speed;
	velocity.z = wish.z * speed;
	if (input.jumpPressed && grounded && !rummaging) {
		velocity.y = 6.5f;
		grounded = false;
	}
	velocity.y -= 17.5f * dt;

	Vector3 candidate = position;
	candidate.x += velocity.x * dt;
	if (!collides(getBoundsAt(candidate), obstacles)) position.x = candidate.x;
	candidate = position;
	candidate.z += velocity.z * dt;
	if (!collides(getBoundsAt(candidate), obstacles)) position.z = candidate.z;
	candidate = position;
	candidate.y += velocity.y * dt;
	if (!collides(getBoundsAt(candidate), obstacles)) position.y = candidate.y;
	else velocity.y = 0.0f;
	if (position.y <= BODY_HALF_HEIGHT) {
		position.y = BODY_HALF_HEIGHT;
		velocity.y = 0.0f;
		grounded = true;
	}

	setWardHeld(input.wardDown);
	if (wardActive) {
		ward -= 24.0f * dt;
		wardRegenDelay = 1.25f;
		if (ward <= 0.0f) {
			ward = 0.0f;
			wardActive = false;
		}
	} else {
		wardRegenDelay = std::max(0.0f, wardRegenDelay - dt);
		if (wardRegenDelay <= 0.0f) ward = std::min(maxWard, ward + 14.0f * dt);
	}
	damageFlash = std::max(0.0f, damageFlash - dt);
	castFlash = std::max(0.0f, castFlash - dt);

	camera.position = Vector3{position.x, position.y + EYE_HEIGHT, position.z};
	camera.target = Vector3Add(camera.position, getLookDirection());
}

void Player::addReagent(ReagentType type, int amount) {
	inventory[static_cast<int>(type)] += amount;
}

int Player::reagentCount(ReagentType type) const {
	return inventory[static_cast<int>(type)];
}

bool Player::selectReagent(ReagentType type) {
	int alreadySelected = 0;
	for (int i = 0; i < spell.getReagentCount(); ++i) {
		if (spell.getReagents()[i] == type) alreadySelected++;
	}
	if (inventory[static_cast<int>(type)] <= alreadySelected) return false;
	return spell.addReagent(type);
}

void Player::consumeSelectedReagents() {
	for (int i = 0; i < spell.getReagentCount(); ++i) {
		int& amount = inventory[static_cast<int>(spell.getReagents()[i])];
		amount = std::max(0, amount - 1);
	}
	spell.clearReagents();
}

void Player::setWardHeld(bool held) {
	wardActive = held && ward > 0.0f && isAlive();
}

void Player::gainWard(float amount) {
	ward = std::min(maxWard, ward + amount);
}

void Player::takeDamage(float amount) {
	float healthDamage = amount;
	if (wardActive && ward > 0.0f) {
		const float absorbed = std::min(amount * 0.8f, ward * 0.5f);
		ward -= absorbed * 2.0f;
		healthDamage -= absorbed;
		wardRegenDelay = 2.0f;
	}
	damage(healthDamage);
	damageFlash = 0.28f;
}
