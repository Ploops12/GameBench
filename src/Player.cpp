#include <algorithm>
#include <cmath>
#include "Player.hxx"
#include <raymath.h>

namespace {
	constexpr float kLookSensitivity = 0.09f;
	constexpr float kWalkSpeed = 5.6f;
	constexpr float kSprintSpeed = 8.7f;
	constexpr float kRummageSpeed = 2.4f;
	constexpr float kJumpSpeed = 7.0f;
	constexpr float kGravity = 22.0f;

	constexpr std::array<BaseVectorType, 4> kBaseCycleOrder {
		BaseVectorType::Bottle,
		BaseVectorType::Apple,
		BaseVectorType::Spider,
		BaseVectorType::Poppet
	};

	int cycleIndexForBase(BaseVectorType base) {
		for (int i = 0; i < static_cast<int>(kBaseCycleOrder.size()); ++i) {
			if (kBaseCycleOrder[i] == base) {
				return i;
			}
		}

		return 0;
	}
}

Player::Player()
	: Actor({}, 0.42f, 100.0f) {
	camera.fovy = 75.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	reset(Vector3 {0.0f, 0.0f, 12.0f});
}

void Player::reset(Vector3 spawnPosition) {
	position = spawnPosition;
	velocity = {};
	health = maxHealth;
	grounded = true;
	rummaging = false;
	wardActive = false;
	wardCharge = maxWard;
	wardCooldownRemaining = 0.0f;
	wardRegenDelay = 0.0f;
	castCooldownRemaining = 0.0f;
	currentCurse = CurseVector {};
	reagentInventory.fill(0);
	syncCamera();
}

void Player::update(float dt, const InputHandler::InputState& input, const std::vector<BoundingBox>& blockers) {
	if (!active) {
		return;
	}

	rummaging = input.rummageHeld;
	if (input.mouseCaptured) {
		yaw += input.lookInput.x * kLookSensitivity;
		pitch -= input.lookInput.y * kLookSensitivity;
		pitch = std::clamp(pitch, -85.0f, 85.0f);
	}

	if (input.mouseWheel != 0.0f && rummaging) {
		const int current = cycleIndexForBase(currentCurse.getBase());
		const int direction = input.mouseWheel > 0.0f ? 1 : -1;
		int next = current + direction;
		if (next < 0) {
			next = 3;
		}
		if (next > 3) {
			next = 0;
		}
		currentCurse.setBase(kBaseCycleOrder[next]);
	}

	updateMovement(dt, input, blockers);
	updateWard(dt, input.wardHeld);
	currentCurse.pruneToInventory(reagentInventory);
	castCooldownRemaining = std::max(0.0f, castCooldownRemaining - dt);
	castFlash = std::max(0.0f, castFlash - dt);

	if (handsSprite && handsSprite->isLoaded()) {
		handsAnimation.update(dt);
	}
	if (wardSprite && wardSprite->isLoaded()) {
		wardAnimation.update(dt);
	}

	syncCamera();
}

void Player::draw3D(const Camera3D&) const {
}

void Player::drawHands2D(int screenWidth, int screenHeight) const {
	if (!handsSprite || !handsSprite->isLoaded()) {
		return;
	}

	int row = 0;
	int column = 1;
	if (rummaging) {
		row = 1;
		column = 3;
	} else if (wardActive) {
		row = 2;
		column = 5;
	} else if (castFlash > 0.0f) {
		row = 0;
		column = 6;
	}

	Rectangle source = handsSprite->frame(8, 3, row * 8 + column);
	const float scale = 1.7f;
	const Rectangle destination {
		static_cast<float>(screenWidth / 2) - source.width * scale * 0.5f,
		static_cast<float>(screenHeight) - source.height * scale * 0.72f,
		source.width * scale,
		source.height * scale
	};
	DrawTexturePro(handsSprite->getTexture(), source, destination, Vector2 {}, 0.0f, WHITE);

	if (wardActive && wardSprite && wardSprite->isLoaded()) {
		const Rectangle sigilSource = wardAnimation.currentFrame();
		const float sigilScale = 0.75f;
		const Rectangle sigilDestination {
			static_cast<float>(screenWidth / 2) - sigilSource.width * sigilScale * 0.5f,
			static_cast<float>(screenHeight) - 250.0f,
			sigilSource.width * sigilScale,
			sigilSource.height * sigilScale
		};
		DrawTexturePro(wardAnimation.texture(), sigilSource, sigilDestination, Vector2 {}, 0.0f, ColorAlpha(WHITE, 0.75f));
	}
}

void Player::setHandsSprite(Sprite* sprite) {
	handsSprite = sprite;
	handsAnimation.setSprite(sprite);
	handsAnimation.setGrid(8, 3);
	handsAnimation.setPlayback(8.0f, 0, 8, 0);
}

void Player::setWardSprite(Sprite* sprite) {
	wardSprite = sprite;
	wardAnimation.setSprite(sprite);
	wardAnimation.setGrid(10, 1);
	wardAnimation.setPlayback(12.0f, 0, 10, 0);
}

const Camera3D& Player::getCamera() const {
	return camera;
}

Vector3 Player::getForward() const {
	const float yawRadians = DEG2RAD * yaw;
	const float pitchRadians = DEG2RAD * pitch;
	return Vector3Normalize(Vector3 {
		std::cos(yawRadians) * std::cos(pitchRadians),
		std::sin(pitchRadians),
		std::sin(yawRadians) * std::cos(pitchRadians)
	});
}

Vector3 Player::getRight() const {
	return Vector3Normalize(Vector3CrossProduct(getForward(), Vector3 {0.0f, 1.0f, 0.0f}));
}

bool Player::isGrounded() const {
	return grounded;
}

bool Player::isRummaging() const {
	return rummaging;
}

bool Player::isWardActive() const {
	return wardActive;
}

bool Player::isAlive() const {
	return Actor::isAlive();
}

float Player::getWard() const {
	return wardCharge;
}

float Player::getMaxWard() const {
	return maxWard;
}

float Player::getCastCooldownRemaining() const {
	return castCooldownRemaining;
}

float Player::getWardCooldownRemaining() const {
	return wardCooldownRemaining;
}

const std::array<int, ReagentTypeCount>& Player::inventory() const {
	return reagentInventory;
}

void Player::collect(ReagentType type, int amount) {
	reagentInventory[static_cast<int>(type)] += amount;
}

CurseVector& Player::spellBuilder() {
	return currentCurse;
}

const CurseVector& Player::spellBuilder() const {
	return currentCurse;
}

SpellPreview Player::previewSpell() const {
	return currentCurse.buildPreview(true);
}

bool Player::canCast() const {
	return castCooldownRemaining <= 0.0f;
}

void Player::triggerCastCooldown(float duration) {
	castCooldownRemaining = duration;
	castFlash = 0.18f;
}

bool Player::consumeForSpell(const SpellPreview& spell) {
	if (spell.fallbackPoppet || spell.totalSelections <= 0) {
		return true;
	}

	for (int i = 0; i < ReagentTypeCount; ++i) {
		if (spell.stacks[i] > reagentInventory[i]) {
			return false;
		}
	}

	for (int i = 0; i < ReagentTypeCount; ++i) {
		reagentInventory[i] -= spell.stacks[i];
	}
	currentCurse.pruneToInventory(reagentInventory);
	return true;
}

void Player::addWard(float amount) {
	wardCharge = std::clamp(wardCharge + amount, 0.0f, maxWard);
}

bool Player::applyIncomingDamage(float amount) {
	float remaining = amount;
	if (wardActive && wardCharge > 0.0f) {
		const float absorbed = std::min(wardCharge, remaining);
		wardCharge -= absorbed;
		remaining -= absorbed;
		wardRegenDelay = 1.2f;
		if (wardCharge <= 0.0f) {
			wardCooldownRemaining = 0.65f;
			wardActive = false;
		}
	}

	if (remaining > 0.0f) {
		return takeDamage(remaining);
	}

	return false;
}

void Player::updateMovement(float dt, const InputHandler::InputState& input, const std::vector<BoundingBox>& blockers) {
	Vector3 forward = getForward();
	forward.y = 0.0f;
	forward = Vector3Normalize(forward);

	Vector3 right = getRight();
	right.y = 0.0f;
	right = Vector3Normalize(right);

	Vector3 desired {};
	desired = Vector3Add(desired, Vector3Scale(forward, input.moveInput.x));
	desired = Vector3Add(desired, Vector3Scale(right, input.moveInput.y));
	if (Vector3Length(desired) > 0.01f) {
		desired = Vector3Normalize(desired);
	}

	float speed = kWalkSpeed;
	if (rummaging) {
		speed = kRummageSpeed;
	} else if (input.sprintHeld) {
		speed = kSprintSpeed;
	}

	velocity.x = desired.x * speed;
	velocity.z = desired.z * speed;

	if (grounded && input.jumpPressed && !rummaging) {
		velocity.y = kJumpSpeed;
		grounded = false;
	}

	velocity.y -= kGravity * dt;

	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
	position.z += velocity.z * dt;

	if (position.y <= 0.0f) {
		position.y = 0.0f;
		velocity.y = 0.0f;
		grounded = true;
	}

	resolveCollisions(blockers);

	position.x = std::clamp(position.x, -22.5f, 22.5f);
	position.z = std::clamp(position.z, -34.0f, 34.0f);
}

void Player::updateWard(float dt, bool wardHeld) {
	if (wardCooldownRemaining > 0.0f) {
		wardCooldownRemaining = std::max(0.0f, wardCooldownRemaining - dt);
	}

	if (wardHeld && wardCooldownRemaining <= 0.0f && wardCharge > 0.0f) {
		wardActive = true;
		wardCharge = std::max(0.0f, wardCharge - dt * 18.0f);
		wardRegenDelay = 1.15f;
		if (wardCharge <= 0.0f) {
			wardCooldownRemaining = 0.65f;
			wardActive = false;
		}
	} else {
		wardActive = false;
		if (wardRegenDelay > 0.0f) {
			wardRegenDelay = std::max(0.0f, wardRegenDelay - dt);
		} else {
			wardCharge = std::min(maxWard, wardCharge + dt * 12.0f);
		}
	}
}

void Player::resolveCollisions(const std::vector<BoundingBox>& blockers) {
	for (const BoundingBox& box : blockers) {
		const float closestX = std::clamp(position.x, box.min.x, box.max.x);
		const float closestZ = std::clamp(position.z, box.min.z, box.max.z);
		const float dx = position.x - closestX;
		const float dz = position.z - closestZ;
		const float distanceSquared = dx * dx + dz * dz;
		const float radiusSquared = collisionRadius * collisionRadius;
		if (distanceSquared >= radiusSquared) {
			continue;
		}

		float distance = std::sqrt(std::max(distanceSquared, 0.0001f));
		float push = collisionRadius - distance;
		Vector3 normal {dx / distance, 0.0f, dz / distance};
		position = Vector3Add(position, Vector3Scale(normal, push));
	}
}

void Player::syncCamera() {
	camera.position = Vector3 {position.x, position.y + eyeHeight, position.z};
	camera.target = Vector3Add(camera.position, getForward());
	camera.up = Vector3 {0.0f, 1.0f, 0.0f};
}
