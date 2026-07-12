#include "Player.hxx"

#include <raylib.h>
#include <raymath.h>

#include <algorithm>
#include <cmath>

namespace {
	constexpr float GRAVITY = 19.0f;
	constexpr float PLAYER_HEIGHT = 1.7f;
	constexpr float PLAYER_RADIUS = 0.38f;
	constexpr Color HAND_LINE{224, 230, 214, 255};
	constexpr Color HAND_SHADOW{19, 20, 30, 235};
	constexpr Color HAND_FILL{49, 42, 52, 250};
	constexpr Color WARD_LINE{102, 206, 221, 255};

	bool touchesBoxXZ(const BoundingBox& box, Vector3 candidate, float radius, float height) {
		if (candidate.y > box.max.y || candidate.y + height < box.min.y) return false;
		const float closestX = std::clamp(candidate.x, box.min.x, box.max.x);
		const float closestZ = std::clamp(candidate.z, box.min.z, box.max.z);
		const float dx = candidate.x - closestX;
		const float dz = candidate.z - closestZ;
		return dx * dx + dz * dz < radius * radius;
	}

	Vector2 handPoint(Vector2 origin, float length, float angle) {
		return {origin.x + std::cos(angle) * length, origin.y + std::sin(angle) * length};
	}
}

Player::Player() : Actor({0.0f, 0.0f, 8.0f}) {
	radius = PLAYER_RADIUS;
	maximumHealth = 100.0f;
	currentHealth = maximumHealth;
	viewYaw = PI;
	viewCamera.position = {position.x, position.y + eyeHeight, position.z};
	viewCamera.target = Vector3Add(viewCamera.position, {0.0f, 0.0f, -1.0f});
	viewCamera.up = {0.0f, 1.0f, 0.0f};
	viewCamera.fovy = 70.0f;
	viewCamera.projection = CAMERA_PERSPECTIVE;
}

void Player::update(float deltaTime, const InputHandler::InputState& input, const std::vector<BoundingBox>& obstacles) {
	viewYaw += input.lookInput.x * 0.0026f;
	viewPitch = std::clamp(viewPitch - input.lookInput.y * 0.0026f, -1.15f, 1.15f);

	const bool rummaging = input.rummageHeld;
	const bool sprinting = input.sprintHeld && !rummaging && input.moveInput.x > 0.0f;
	float speed = sprinting ? 8.4f : 5.8f;
	if (rummaging) speed *= 0.38f;
	if (hasteTimer > 0.0f) speed *= 1.16f;

	const float sinYaw = std::sin(viewYaw);
	const float cosYaw = std::cos(viewYaw);
	Vector3 moveDirection{
		sinYaw * input.moveInput.x + cosYaw * input.moveInput.y,
		0.0f,
		cosYaw * input.moveInput.x - sinYaw * input.moveInput.y
	};
	const float moveLength = std::sqrt(moveDirection.x * moveDirection.x + moveDirection.z * moveDirection.z);
	if (moveLength > 0.001f) {
		moveDirection.x /= moveLength;
		moveDirection.z /= moveLength;
	}
	velocity.x = moveDirection.x * speed;
	velocity.z = moveDirection.z * speed;

	if (input.jumpPressed && onGround && !rummaging) {
		velocity.y = 7.0f;
		onGround = false;
	}
	velocity.y -= GRAVITY * deltaTime;
	position.y += velocity.y * deltaTime;
	if (position.y <= 0.0f) {
		position.y = 0.0f;
		velocity.y = 0.0f;
		onGround = true;
	}

	Vector3 candidate = position;
	candidate.x += velocity.x * deltaTime;
	resolveHorizontalCollision(candidate, obstacles);
	candidate.z += velocity.z * deltaTime;
	resolveHorizontalCollision(candidate, obstacles);
	position.x = std::clamp(candidate.x, -14.25f, 14.25f);
	position.z = std::clamp(candidate.z, -11.25f, 11.25f);

	if (wardShieldActive) {
		if (!input.wardHeld || wardValue <= 0.0f) {
			wardShieldActive = false;
			wardLockout = 1.15f;
		} else {
			wardValue = std::max(0.0f, wardValue - 16.0f * deltaTime);
		}
	} else {
		wardLockout = std::max(0.0f, wardLockout - deltaTime);
		if (input.wardHeld && wardLockout <= 0.0f && wardValue > 0.0f) {
			wardShieldActive = true;
		} else if (!input.wardHeld && wardLockout <= 0.0f) {
			wardValue = std::min(maximumWard, wardValue + 10.0f * deltaTime);
		}
	}
	if (hasteTimer > 0.0f) hasteTimer = std::max(0.0f, hasteTimer - deltaTime);
	if (regenerationTimer > 0.0f) {
		heal(regenerationRate * deltaTime);
		regenerationTimer = std::max(0.0f, regenerationTimer - deltaTime);
	}
	if (damageFlash > 0.0f) damageFlash = std::max(0.0f, damageFlash - deltaTime);

	viewCamera.position = {position.x, position.y + eyeHeight, position.z};
	viewCamera.target = Vector3Add(viewCamera.position, forward());
}

void Player::resolveHorizontalCollision(Vector3& candidate, const std::vector<BoundingBox>& obstacles) const {
	for (const BoundingBox& obstacle : obstacles) {
		if (!touchesBoxXZ(obstacle, candidate, PLAYER_RADIUS, PLAYER_HEIGHT)) continue;
		if (std::abs(candidate.x - position.x) > 0.001f) candidate.x = position.x;
		if (std::abs(candidate.z - position.z) > 0.001f) candidate.z = position.z;
	}
}

void Player::takeDamage(float amount) {
	if (!isAlive()) return;
	float remaining = std::max(0.0f, amount);
	if (wardShieldActive && wardValue > 0.0f) {
		const float absorbed = std::min(wardValue, remaining);
		wardValue -= absorbed;
		remaining -= absorbed;
		if (wardValue <= 0.0f) {
			wardShieldActive = false;
			wardLockout = 1.4f;
		}
	}
	if (remaining > 0.0f) {
		currentHealth = std::max(0.0f, currentHealth - remaining);
		if (currentHealth <= 0.0f) active = false;
	}
	damageFlash = 0.3f;
}

void Player::drawHands(int screenWidth, int screenHeight, bool rummaging, bool casting) const {
	const float width = static_cast<float>(screenWidth);
	const float height = static_cast<float>(screenHeight);
	const Color accent = wardShieldActive ? WARD_LINE : (rummaging ? Color{224, 154, 90, 255} : HAND_LINE);
	const float sway = std::sin(static_cast<float>(GetTime()) * 2.0f) * 3.0f;
	const Vector2 leftPalm{width * 0.205f + sway, height * 0.83f};
	const Vector2 rightPalm{width * 0.795f - sway, height * 0.83f};

	DrawLineEx({width * 0.015f, height * 1.06f}, leftPalm, 80.0f, HAND_SHADOW);
	DrawLineEx({width * 0.985f, height * 1.06f}, rightPalm, 80.0f, HAND_SHADOW);
	DrawLineEx({width * 0.015f, height * 1.06f}, leftPalm, 48.0f, HAND_FILL);
	DrawLineEx({width * 0.985f, height * 1.06f}, rightPalm, 48.0f, HAND_FILL);

	DrawCircleV(leftPalm, 34.0f, HAND_FILL);
	DrawCircleV(rightPalm, 34.0f, HAND_FILL);
	DrawCircleLines(static_cast<int>(leftPalm.x), static_cast<int>(leftPalm.y), 35.0f, accent);
	DrawCircleLines(static_cast<int>(rightPalm.x), static_cast<int>(rightPalm.y), 35.0f, accent);

	const float leftAngles[5] = {-2.4f, -2.0f, -1.65f, -1.3f, -0.95f};
	const float rightAngles[5] = {-2.2f, -1.85f, -1.5f, -1.15f, -0.8f};
	for (int i = 0; i < 5; ++i) {
		const float length = 30.0f + static_cast<float>(i % 2) * 5.0f;
		DrawLineEx(handPoint(leftPalm, 20.0f, leftAngles[i]), handPoint(leftPalm, length, leftAngles[i] - 0.08f), 7.0f, HAND_FILL);
		DrawLineEx(handPoint(leftPalm, 20.0f, leftAngles[i]), handPoint(leftPalm, length, leftAngles[i] - 0.08f), 2.0f, accent);
		DrawLineEx(handPoint(rightPalm, 20.0f, rightAngles[i]), handPoint(rightPalm, length, rightAngles[i] - 0.08f), 7.0f, HAND_FILL);
		DrawLineEx(handPoint(rightPalm, 20.0f, rightAngles[i]), handPoint(rightPalm, length, rightAngles[i] - 0.08f), 2.0f, accent);
	}

	DrawLineEx({leftPalm.x - 14.0f, leftPalm.y + 8.0f}, {leftPalm.x + 18.0f, leftPalm.y + 24.0f}, 2.0f, accent);
	DrawLineEx({rightPalm.x + 14.0f, rightPalm.y + 8.0f}, {rightPalm.x - 18.0f, rightPalm.y + 24.0f}, 2.0f, accent);
	DrawCircleLines(static_cast<int>(leftPalm.x), static_cast<int>(leftPalm.y), 14.0f, ColorAlpha(accent, 0.55f));
	DrawCircleLines(static_cast<int>(rightPalm.x), static_cast<int>(rightPalm.y), 14.0f, ColorAlpha(accent, 0.55f));

	if (casting) {
		DrawLineEx({leftPalm.x, leftPalm.y - 32.0f}, {leftPalm.x, leftPalm.y - 72.0f}, 3.0f, accent);
		DrawLineEx({rightPalm.x, rightPalm.y - 32.0f}, {rightPalm.x, rightPalm.y - 72.0f}, 3.0f, accent);
		DrawCircleLines(static_cast<int>(width * 0.5f), static_cast<int>(height * 0.52f), 22.0f, accent);
	}

	if (wardShieldActive) {
		DrawRing({width * 0.5f, height * 0.52f}, 80.0f, 86.0f, 0.0f, 360.0f, 48, ColorAlpha(WARD_LINE, 0.24f));
	}
}

const Camera3D& Player::camera() const {
	return viewCamera;
}

Vector3 Player::forward() const {
	const float cosPitch = std::cos(viewPitch);
	return {std::sin(viewYaw) * cosPitch, std::sin(viewPitch), std::cos(viewYaw) * cosPitch};
}

Vector3 Player::right() const {
	return {std::cos(viewYaw), 0.0f, -std::sin(viewYaw)};
}

float Player::yaw() const {
	return viewYaw;
}

float Player::pitch() const {
	return viewPitch;
}

bool Player::grounded() const {
	return onGround;
}

void Player::addReagent(ReagentType type, int amount) {
	if (amount <= 0) return;
	reagents[ReagentIndex(type)] += amount;
}

bool Player::consumeReagent(ReagentType type, int amount) {
	if (amount <= 0) return true;
	const int index = ReagentIndex(type);
	if (reagents[index] < amount) return false;
	reagents[index] -= amount;
	return true;
}

int Player::reagentQuantity(ReagentType type) const {
	return reagents[ReagentIndex(type)];
}

int Player::totalReagents() const {
	int total = 0;
	for (int quantity : reagents) total += quantity;
	return total;
}

float Player::ward() const {
	return wardValue;
}

float Player::maxWard() const {
	return maximumWard;
}

bool Player::wardActive() const {
	return wardShieldActive;
}

float Player::wardCooldown() const {
	return wardLockout;
}

void Player::addWard(float amount) {
	wardValue = std::min(maximumWard, wardValue + std::max(0.0f, amount));
}

void Player::heal(float amount) {
	currentHealth = std::min(maximumHealth, currentHealth + std::max(0.0f, amount));
}

void Player::setHaste(float duration) {
	hasteTimer = std::max(hasteTimer, duration);
}

void Player::setRegeneration(float duration, float rate) {
	regenerationTimer = std::max(regenerationTimer, duration);
	regenerationRate = std::max(regenerationRate, rate);
}

float Player::hasteTime() const {
	return hasteTimer;
}

float Player::hurtFlash() const {
	return damageFlash;
}
