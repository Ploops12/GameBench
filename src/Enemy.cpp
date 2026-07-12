#include "Enemy.hxx"
#include <algorithm>
#include <cmath>
#include <raymath.h>

namespace {
bool collides(BoundingBox box, const std::vector<BoundingBox>& obstacles) {
	for (const BoundingBox& obstacle : obstacles) {
		if (CheckCollisionBoxes(box, obstacle)) return true;
	}
	return false;
}
}

Enemy::Enemy(Vector3 position, int kind) :
	Actor(position, 0.48f, 42.0f + kind * 9.0f), kind(kind) {}

BoundingBox Enemy::getBoundsAt(Vector3 value) const {
	return BoundingBox{
		Vector3{value.x - 0.38f, 0.0f, value.z - 0.38f},
		Vector3{value.x + 0.38f, 1.9f, value.z + 0.38f}
	};
}

void Enemy::update(float dt, Vector3 playerPosition, const std::vector<BoundingBox>& obstacles) {
	attackCooldown -= dt;
	snareTimer = std::max(0.0f, snareTimer - dt);
	fearTimer = std::max(0.0f, fearTimer - dt);
	flashTimer = std::max(0.0f, flashTimer - dt);
	if (rotTimer > 0.0f) {
		rotTimer -= dt;
		damage(rotDamagePerSecond * dt);
	}
	if (!isAlive()) return;

	Vector3 direction = Vector3Subtract(playerPosition, position);
	direction.y = 0.0f;
	const float distance = Vector3Length(direction);
	if (distance > 0.01f) direction = Vector3Scale(direction, 1.0f / distance);
	if (fearTimer > 0.0f) direction = Vector3Negate(direction);
	const float snareScale = snareTimer > 0.0f ? 0.28f : 1.0f;
	const float speed = (1.45f + kind * 0.12f) * snareScale;
	if (distance > 1.15f || fearTimer > 0.0f) {
		Vector3 candidate = position;
		candidate.x += direction.x * speed * dt;
		if (!collides(getBoundsAt(candidate), obstacles)) position.x = candidate.x;
		candidate = position;
		candidate.z += direction.z * speed * dt;
		if (!collides(getBoundsAt(candidate), obstacles)) position.z = candidate.z;
	}
}

void Enemy::applyHex(const SpellStats& stats) {
	damage(stats.damage);
	snareTimer = std::max(snareTimer, stats.snareDuration);
	fearTimer = std::max(fearTimer, stats.fearDuration);
	if (stats.rotDamagePerSecond > 0.0f) {
		rotDamagePerSecond = std::max(rotDamagePerSecond, stats.rotDamagePerSecond);
		rotTimer = std::max(rotTimer, 2.5f + stats.lingeringDuration);
	}
	flashTimer = 0.16f;
}

void Enemy::draw3D() const {
	const Color base = kind % 2 == 0 ? Color{235, 100, 75, 255} : Color{235, 175, 75, 255};
	const Color color = flashTimer > 0.0f ? WHITE : (snareTimer > 0.0f ? Color{100, 220, 130, 255} : base);
	const Vector3 feet = Vector3{position.x, 0.05f, position.z};
	const Vector3 chest = Vector3{position.x, 1.25f, position.z};
	const Vector3 head = Vector3{position.x, 1.75f, position.z};
	DrawLine3D(feet, chest, color);
	DrawLine3D(Vector3{position.x - 0.55f, 1.15f, position.z}, Vector3{position.x + 0.55f, 1.15f, position.z}, color);
	DrawLine3D(feet, Vector3{position.x - 0.35f, 0.0f, position.z}, color);
	DrawLine3D(feet, Vector3{position.x + 0.35f, 0.0f, position.z}, color);
	DrawSphereWires(head, 0.24f, 5, 6, color);
	DrawTriangle3D(Vector3{position.x - 0.45f, 1.92f, position.z}, Vector3{position.x + 0.45f, 1.92f, position.z},
		Vector3{position.x, 2.25f, position.z}, color);
	if (fearTimer > 0.0f) DrawCircle3D(Vector3{position.x, 2.45f, position.z}, 0.18f, Vector3{1, 0, 0}, 90.0f, Color{205, 175, 95, 255});

	const float healthRatio = health / maxHealth;
	DrawLine3D(Vector3{position.x - 0.45f, 2.35f, position.z}, Vector3{position.x + 0.45f, 2.35f, position.z}, Color{50, 40, 45, 255});
	DrawLine3D(Vector3{position.x - 0.45f, 2.35f, position.z}, Vector3{position.x - 0.45f + 0.9f * healthRatio, 2.35f, position.z}, color);
}
