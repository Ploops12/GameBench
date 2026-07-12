#include "Player.hxx"

#include <raymath.h>

#include <algorithm>
#include <cmath>

namespace {
constexpr float PLAYER_RADIUS = 0.34f;
constexpr float PLAYER_HEIGHT = 1.78f;
constexpr float EYE_HEIGHT = 1.62f;
constexpr float GRAVITY = 23.0f;
constexpr float JUMP_SPEED = 8.4f;
constexpr float WALK_SPEED = 6.4f;
constexpr float SPRINT_SPEED = 10.2f;
constexpr float RUMMAGE_SPEED = 3.25f;
constexpr float MOUSE_SENSITIVITY = 0.00225f;

float MoveToward(float current, float target, float maxDelta) {
	if (std::fabs(target - current) <= maxDelta) {
		return target;
	}
	return current + std::copysign(maxDelta, target - current);
}
}

Player::Player(Vector3 spawnPosition)
	: Actor(spawnPosition, 100.0f, PLAYER_RADIUS, PLAYER_HEIGHT), spawnPosition_(spawnPosition) {
	camera_.position = {spawnPosition.x, spawnPosition.y + EYE_HEIGHT, spawnPosition.z};
	camera_.target = {spawnPosition.x, spawnPosition.y + EYE_HEIGHT, spawnPosition.z - 1.0f};
	camera_.up = {0.0f, 1.0f, 0.0f};
	camera_.fovy = 72.0f;
	camera_.projection = CAMERA_PERSPECTIVE;
}

void Player::reset(Vector3 spawnPosition) {
	spawnPosition_ = spawnPosition;
	position_ = spawnPosition;
	velocity_ = {};
	restoreHealth();
	inventory_.fill(0);
	yaw_ = 0.0f;
	pitch_ = 0.0f;
	ward_ = maxWard_;
	wardRegenDelay_ = 0.0f;
	castCooldown_ = 0.0f;
	castAnimation_ = 0.0f;
	hitMarker_ = 0.0f;
	damageFlash_ = 0.0f;
	invulnerabilityTimer_ = 0.0f;
	bobPhase_ = 0.0f;
	movementBob_ = 0.0f;
	grounded_ = true;
	wardActive_ = false;
	updateCamera(0.0f, false);
}

void Player::updatePlayer(const InputHandler::InputState& input, float dt,
	const std::vector<BoundingBox>& obstacles, bool rummaging) {
	castCooldown_ = std::max(0.0f, castCooldown_ - dt);
	castAnimation_ = std::max(0.0f, castAnimation_ - dt * 4.2f);
	hitMarker_ = std::max(0.0f, hitMarker_ - dt * 4.0f);
	damageFlash_ = std::max(0.0f, damageFlash_ - dt * 2.5f);
	invulnerabilityTimer_ = std::max(0.0f, invulnerabilityTimer_ - dt);

	if (!isAlive()) {
		wardActive_ = false;
		return;
	}

	yaw_ += input.lookInput.x * MOUSE_SENSITIVITY;
	pitch_ -= input.lookInput.y * MOUSE_SENSITIVITY;
	pitch_ = std::clamp(pitch_, -1.42f, 1.42f);

	wardActive_ = input.wardHeld && ward_ > 0.0f;
	if (wardActive_) {
		ward_ = std::max(0.0f, ward_ - 22.0f * dt);
		wardRegenDelay_ = 1.35f;
		if (ward_ <= 0.0f) {
			wardActive_ = false;
			wardRegenDelay_ = 2.2f;
		}
	} else if (wardRegenDelay_ > 0.0f) {
		wardRegenDelay_ = std::max(0.0f, wardRegenDelay_ - dt);
	} else {
		ward_ = std::min(maxWard_, ward_ + 15.0f * dt);
	}

	const bool sprinting = input.sprintHeld && !rummaging && input.moveInput.x > 0.0f && grounded_;
	const float maxSpeed = rummaging ? RUMMAGE_SPEED : (sprinting ? SPRINT_SPEED : WALK_SPEED);
	const Vector3 forward = flatForward();
	const Vector3 right {std::cos(yaw_), 0.0f, std::sin(yaw_)};
	Vector3 desired = Vector3Add(Vector3Scale(forward, input.moveInput.x), Vector3Scale(right, input.moveInput.y));
	if (Vector3LengthSqr(desired) > 1.0f) {
		desired = Vector3Normalize(desired);
	}
	desired = Vector3Scale(desired, maxSpeed);

	const float acceleration = grounded_ ? 52.0f : 17.0f;
	velocity_.x = MoveToward(velocity_.x, desired.x, acceleration * dt);
	velocity_.z = MoveToward(velocity_.z, desired.z, acceleration * dt);

	if (input.jumpPressed && grounded_ && !rummaging) {
		velocity_.y = JUMP_SPEED;
		grounded_ = false;
	}
	velocity_.y -= GRAVITY * dt;

	moveAndCollide(Vector3Scale(velocity_, dt), obstacles);
	if (position_.y <= 0.0f) {
		position_.y = 0.0f;
		if (velocity_.y < 0.0f) {
			velocity_.y = 0.0f;
		}
		grounded_ = true;
	}

	const float speed = horizontalSpeed();
	if (grounded_ && speed > 0.2f) {
		bobPhase_ += dt * (sprinting ? 15.0f : 10.5f) * std::clamp(speed / WALK_SPEED, 0.5f, 1.6f);
		movementBob_ = std::sin(bobPhase_) * (sprinting ? 0.050f : 0.030f);
	} else {
		movementBob_ = MoveToward(movementBob_, 0.0f, dt * 0.2f);
	}

	updateCamera(dt, sprinting);
}

const Camera3D& Player::camera() const {
	return camera_;
}

Vector3 Player::viewDirection() const {
	return {
		std::cos(pitch_) * std::sin(yaw_),
		std::sin(pitch_),
		-std::cos(pitch_) * std::cos(yaw_)
	};
}

Vector3 Player::flatForward() const {
	return {std::sin(yaw_), 0.0f, -std::cos(yaw_)};
}

bool Player::isGrounded() const {
	return grounded_;
}

float Player::horizontalSpeed() const {
	return std::sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
}

float Player::takeIncomingDamage(float damage, bool vulnerable) {
	if (!isAlive() || damage <= 0.0f || invulnerabilityTimer_ > 0.0f) {
		return 0.0f;
	}
	if (vulnerable) {
		damage *= 1.25f;
	}
	const float incoming = damage;
	if (wardActive_ && ward_ > 0.0f) {
		const float absorbed = std::min(damage, ward_ / 1.25f);
		ward_ -= absorbed * 1.25f;
		damage -= absorbed;
		wardRegenDelay_ = ward_ > 0.0f ? 2.0f : 2.5f;
	}
	const float healthDamage = Actor::takeDamage(damage);
	damageFlash_ = std::max(damageFlash_, healthDamage > 0.0f ? 1.0f : 0.35f);
	invulnerabilityTimer_ = 0.24f;
	return std::min(incoming, healthDamage);
}

void Player::addWard(float amount) {
	ward_ = std::clamp(ward_ + amount, 0.0f, maxWard_);
}

float Player::ward() const {
	return ward_;
}

float Player::maxWard() const {
	return maxWard_;
}

bool Player::isWardActive() const {
	return wardActive_;
}

float Player::wardRegenDelay() const {
	return wardRegenDelay_;
}

void Player::collect(ReagentType type, int amount) {
	if (type == ReagentType::Count || amount <= 0) {
		return;
	}
	int& quantity = inventory_[ReagentIndex(type)];
	quantity = std::min(99, quantity + amount);
}

int Player::reagentQuantity(ReagentType type) const {
	if (type == ReagentType::Count) {
		return 0;
	}
	return inventory_[ReagentIndex(type)];
}

const ReagentInventory& Player::inventory() const {
	return inventory_;
}

bool Player::consumeFor(const CurseVector& spell) {
	if (!spell.canConsume(inventory_)) {
		return false;
	}
	for (const ReagentType reagent : spell.reagents()) {
		--inventory_[ReagentIndex(reagent)];
	}
	return true;
}

bool Player::hasAnyReagents() const {
	return std::any_of(inventory_.begin(), inventory_.end(), [](int quantity) {
		return quantity > 0;
	});
}

bool Player::canCast() const {
	return isAlive() && castCooldown_ <= 0.0f;
}

void Player::beginCast(float cooldown) {
	castCooldown_ = std::max(0.05f, cooldown);
	castAnimation_ = 1.0f;
}

void Player::registerHit() {
	hitMarker_ = 1.0f;
}

float Player::castAnimation() const {
	return castAnimation_;
}

float Player::hitMarker() const {
	return hitMarker_;
}

float Player::damageFlash() const {
	return damageFlash_;
}

float Player::movementBob() const {
	return movementBob_;
}

void Player::updateCamera(float dt, bool sprinting) {
	const float eyeBob = grounded_ ? movementBob_ : 0.0f;
	camera_.position = {position_.x, position_.y + EYE_HEIGHT + eyeBob, position_.z};
	camera_.target = Vector3Add(camera_.position, viewDirection());
	const float targetFov = sprinting ? 78.0f : 72.0f;
	const float blend = std::clamp(dt * 8.0f, 0.0f, 1.0f);
	camera_.fovy += (targetFov - camera_.fovy) * blend;
}

void Player::moveAndCollide(Vector3 delta, const std::vector<BoundingBox>& obstacles) {
	Vector3 candidate = position_;
	candidate.x += delta.x;
	if (!collidesAt(candidate, obstacles)) {
		position_.x = candidate.x;
	} else {
		velocity_.x = 0.0f;
	}

	candidate = position_;
	candidate.z += delta.z;
	if (!collidesAt(candidate, obstacles)) {
		position_.z = candidate.z;
	} else {
		velocity_.z = 0.0f;
	}

	position_.y += delta.y;
}

bool Player::collidesAt(Vector3 position, const std::vector<BoundingBox>& obstacles) const {
	const BoundingBox playerBounds {
		{position.x - collisionRadius_, position.y + 0.02f, position.z - collisionRadius_},
		{position.x + collisionRadius_, position.y + height_, position.z + collisionRadius_}
	};
	for (const BoundingBox obstacle : obstacles) {
		if (CheckCollisionBoxes(playerBounds, obstacle)) {
			return true;
		}
	}
	return false;
}
