#include "Player.hxx"

#include <algorithm>

#include <raymath.h>

namespace {
constexpr float PlayerEyeHeight = 1.65f;
constexpr float PlayerRadius = 0.38f;
constexpr float WalkSpeed = 5.25f;
constexpr float SprintSpeed = 8.25f;
constexpr float RummageSpeedMultiplier = 0.44f;
constexpr float Gravity = 21.0f;
constexpr float JumpVelocity = 7.4f;
constexpr float WardDrainPerSecond = 26.0f;
constexpr float WardRegenerationPerSecond = 17.0f;
constexpr float WardRechargeDelay = 0.7f;
}

Player::Player()
	: Actor(Vector3 {0.0f, PlayerEyeHeight, 12.0f}, PlayerRadius, 100.0f) {
	camera.fovy = 72.0f;
	camera.projection = CAMERA_PERSPECTIVE;
	camera.up = Vector3 {0.0f, 1.0f, 0.0f};
	updateCamera();
}

void Player::update(float deltaTime, const InputHandler::InputState& input,
	const std::vector<BoundingBox>& solidBounds, bool rummaging) {
	if (!isAlive()) {
		return;
	}

	castCooldown = std::max(0.0f, castCooldown - deltaTime);
	damageFlash = std::max(0.0f, damageFlash - deltaTime);
	wardRechargeDelay = std::max(0.0f, wardRechargeDelay - deltaTime);
	thornGuardTimer = std::max(0.0f, thornGuardTimer - deltaTime);
	panicStepTimer = std::max(0.0f, panicStepTimer - deltaTime);
	waxShellTimer = std::max(0.0f, waxShellTimer - deltaTime);
	if (thornGuardTimer <= 0.0f) {
		thornRetaliationDamage = 0.0f;
		thornSnareDuration = 0.0f;
	}
	if (panicStepTimer <= 0.0f) {
		panicStepMultiplier = 1.0f;
	}
	if (waxShellTimer <= 0.0f) {
		waxShellReduction = 0.0f;
	}
	if (mendTimer > 0.0f) {
		const float activeTime = std::min(mendTimer, deltaTime);
		heal(mendPerSecond * activeTime);
		mendTimer = std::max(0.0f, mendTimer - deltaTime);
	}
	if (mendTimer <= 0.0f) {
		mendPerSecond = 0.0f;
	}

	if (!rummaging) {
		yaw += input.lookInput.x * 0.0028f;
		pitch = Clamp(pitch - input.lookInput.y * 0.0028f, -1.35f, 1.35f);
	}

	wardActive = input.wardHeld && ward > 0.0f;
	if (wardActive) {
		ward = std::max(0.0f, ward - WardDrainPerSecond * deltaTime);
		wardRechargeDelay = WardRechargeDelay;
		if (ward <= 0.0f) {
			wardActive = false;
		}
	} else if (wardRechargeDelay <= 0.0f) {
		ward = std::min(getMaximumWard(), ward + WardRegenerationPerSecond * deltaTime);
	}

	Vector3 forward = getForward();
	forward.y = 0.0f;
	forward = Vector3Normalize(forward);
	Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, Vector3 {0.0f, 1.0f, 0.0f}));
	Vector3 move = Vector3Add(Vector3Scale(forward, input.moveInput.x), Vector3Scale(right, input.moveInput.y));
	if (Vector3Length(move) > 0.001f) {
		move = Vector3Normalize(move);
	}

	float speed = input.sprintHeld && !rummaging ? SprintSpeed : WalkSpeed;
	if (rummaging) {
		speed *= RummageSpeedMultiplier;
	}
	if (panicStepTimer > 0.0f) {
		speed *= panicStepMultiplier;
	}

	Vector3 horizontalCandidate = position;
	horizontalCandidate.x += move.x * speed * deltaTime;
	if (!collidesWithWorld(horizontalCandidate, solidBounds)) {
		position.x = horizontalCandidate.x;
	}

	horizontalCandidate = position;
	horizontalCandidate.z += move.z * speed * deltaTime;
	if (!collidesWithWorld(horizontalCandidate, solidBounds)) {
		position.z = horizontalCandidate.z;
	}

	if (grounded && input.jumpPressed) {
		verticalVelocity = JumpVelocity;
		grounded = false;
	}

	verticalVelocity -= Gravity * deltaTime;
	position.y += verticalVelocity * deltaTime;
	if (position.y <= PlayerEyeHeight) {
		position.y = PlayerEyeHeight;
		verticalVelocity = 0.0f;
		grounded = true;
	}

	updateCamera();
}

void Player::draw(const Camera3D&) const {
}

void Player::takeDamage(float amount) {
	if (amount <= 0.0f || !isAlive()) {
		return;
	}
	if (waxShellTimer > 0.0f) {
		amount *= 1.0f - waxShellReduction;
	}

	if (wardActive && ward > 0.0f) {
		const float shielded = std::min(ward, amount * 0.8f);
		ward -= shielded;
		amount -= shielded;
		wardRechargeDelay = WardRechargeDelay;
	}

	Actor::takeDamage(amount);
	damageFlash = 0.28f;
}

const Camera3D& Player::getCamera() const {
	return camera;
}

Vector3 Player::getForward() const {
	return Vector3Normalize(Vector3 {
		cosf(yaw) * cosf(pitch),
		sinf(pitch),
		sinf(yaw) * cosf(pitch)
	});
}

Vector3 Player::getCastOrigin() const {
	return Vector3Add(camera.position, Vector3Scale(getForward(), 0.42f));
}

bool Player::isGrounded() const {
	return grounded;
}

bool Player::isWardActive() const {
	return wardActive;
}

float Player::getWard() const {
	return ward;
}

float Player::getMaximumWard() const {
	return 100.0f;
}

float Player::getDamageFlash() const {
	return damageFlash;
}

float Player::getCastCooldown() const {
	return castCooldown;
}

float Player::getThornGuardTime() const {
	return thornGuardTimer;
}

float Player::getThornRetaliationDamage() const {
	return thornGuardTimer > 0.0f ? thornRetaliationDamage : 0.0f;
}

float Player::getThornSnareDuration() const {
	return thornGuardTimer > 0.0f ? thornSnareDuration : 0.0f;
}

float Player::getMendTime() const {
	return mendTimer;
}

float Player::getPanicStepTime() const {
	return panicStepTimer;
}

float Player::getWaxShellTime() const {
	return waxShellTimer;
}

void Player::gainWard(float amount) {
	ward = Clamp(ward + amount, 0.0f, getMaximumWard());
}

void Player::applySelfHex(const SelfHexStats& hex) {
	heal(hex.healthRestore);
	gainWard(hex.wardGain);
	if (hex.thornGuardDuration > 0.0f) {
		thornGuardTimer = std::max(thornGuardTimer, hex.thornGuardDuration);
		thornRetaliationDamage = std::max(thornRetaliationDamage, hex.thornDamage);
		thornSnareDuration = std::max(thornSnareDuration, hex.thornGuardDuration);
	}
	if (hex.mendDuration > 0.0f) {
		mendTimer = std::max(mendTimer, hex.mendDuration);
		mendPerSecond = std::max(mendPerSecond, hex.mendPerSecond);
	}
	if (hex.hasteDuration > 0.0f) {
		panicStepTimer = std::max(panicStepTimer, hex.hasteDuration);
		panicStepMultiplier = std::max(panicStepMultiplier, hex.speedMultiplier);
	}
	if (hex.waxShellDuration > 0.0f) {
		waxShellTimer = std::max(waxShellTimer, hex.waxShellDuration);
		waxShellReduction = std::max(waxShellReduction, hex.damageReduction);
	}
}

bool Player::beginCast(float cooldown) {
	if (!isAlive() || castCooldown > 0.0f) {
		return false;
	}

	castCooldown = cooldown;
	return true;
}

void Player::updateCamera() {
	camera.position = position;
	camera.target = Vector3Add(position, getForward());
	camera.up = Vector3 {0.0f, 1.0f, 0.0f};
}

bool Player::collidesWithWorld(Vector3 candidate, const std::vector<BoundingBox>& solidBounds) const {
	const Vector3 feet = Vector3 {candidate.x, 0.8f, candidate.z};
	for (const BoundingBox& bounds : solidBounds) {
		if (CheckCollisionBoxSphere(bounds, feet, PlayerRadius)) {
			return true;
		}
	}

	return false;
}
