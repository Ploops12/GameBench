#include <algorithm>
#include "Enemy.hxx"
#include <raymath.h>

Enemy::Enemy()
	: Actor({0.0f, 0.0f, 0.0f}, 0.45f, 45.0f, 2.2f),
		attackCooldown(0.0f),
		snareTimer(0.0f),
		fearTimer(0.0f),
		rotTimer(0.0f),
		rotDamagePerSecond(0.0f),
		lootDropped(false) {}

Enemy::Enemy(Vector3 startPosition)
	: Actor(startPosition, 0.45f, 45.0f, 2.2f),
		attackCooldown(0.0f),
		snareTimer(0.0f),
		fearTimer(0.0f),
		rotTimer(0.0f),
		rotDamagePerSecond(0.0f),
		lootDropped(false) {}

void Enemy::updateAI(float dt, const Vector3& playerPosition) {
	if (!isAlive()) {
		return;
	}

	attackCooldown = std::max(0.0f, attackCooldown - dt);
	snareTimer = std::max(0.0f, snareTimer - dt);
	fearTimer = std::max(0.0f, fearTimer - dt);

	if (rotTimer > 0.0f) {
		rotTimer = std::max(0.0f, rotTimer - dt);
		takeDamage(rotDamagePerSecond * dt);
	}

	Vector3 toPlayer = Vector3Subtract(playerPosition, position);
	toPlayer.y = 0.0f;
	float distance = Vector3Length(toPlayer);
	Vector3 moveDir {0.0f, 0.0f, 0.0f};

	if (distance > 0.001f) {
		moveDir = Vector3Scale(toPlayer, 1.0f / distance);
	}

	if (fearTimer > 0.0f) {
		moveDir = Vector3Negate(moveDir);
	}

	float speedScale = snareTimer > 0.0f ? 0.25f : 1.0f;
	if (distance > 0.9f || fearTimer > 0.0f) {
		position = Vector3Add(position, Vector3Scale(moveDir, moveSpeed * speedScale * dt));
	}
	position.y = 0.0f;
}

void Enemy::draw3D(const Camera3D&) const {
	Color tint = {192, 96, 72, 255};
	if (snareTimer > 0.0f) {
		tint = {120, 230, 120, 255};
	} else if (fearTimer > 0.0f) {
		tint = {196, 156, 255, 255};
	} else if (rotTimer > 0.0f) {
		tint = {142, 189, 93, 255};
	}

	Vector3 bodyPos = {position.x, 0.9f, position.z};
	DrawCubeV(bodyPos, {0.18f, 1.6f, 0.18f}, Fade(tint, 0.7f));
	DrawCubeWiresV(bodyPos, {0.2f, 1.65f, 0.2f}, WHITE);
	DrawSphere({position.x, 1.85f, position.z}, 0.2f, Fade(tint, 0.9f));
	DrawLine3D({position.x, 1.45f, position.z}, {position.x, 0.25f, position.z}, WHITE);
}

void Enemy::applySpell(const ComposedSpell& spell, const Vector3& sourcePosition, float scale) {
	if (!isAlive()) {
		return;
	}

	takeDamage(spell.damage * scale);

	if (spell.snareDuration > 0.0f) {
		snareTimer = std::max(snareTimer, spell.snareDuration * scale);
	}
	if (spell.fearDuration > 0.0f) {
		fearTimer = std::max(fearTimer, spell.fearDuration * scale);
	}
	if (spell.rotDuration > 0.0f) {
		rotTimer = std::max(rotTimer, spell.rotDuration * scale);
		rotDamagePerSecond = std::max(rotDamagePerSecond, spell.rotDamagePerSecond * scale);
	}

	Vector3 away = Vector3Subtract(position, sourcePosition);
	away.y = 0.0f;
	if (Vector3LengthSqr(away) > 0.001f) {
		away = Vector3Scale(Vector3Normalize(away), 2.0f + spell.splashRadius * 0.6f);
		position = Vector3Add(position, away);
	}
}

bool Enemy::canAttack(const Vector3& playerPosition) const {
	if (!isAlive() || attackCooldown > 0.0f || fearTimer > 0.0f) {
		return false;
	}
	Vector3 delta = Vector3Subtract(playerPosition, position);
	delta.y = 0.0f;
	return Vector3Length(delta) <= getAttackRange();
}

float Enemy::consumeAttack() {
	attackCooldown = 0.9f;
	return 10.0f;
}

bool Enemy::hasDroppedLoot() const {
	return lootDropped;
}

void Enemy::markLootDropped() {
	lootDropped = true;
}

float Enemy::getAttackRange() const {
	return 1.25f;
}
