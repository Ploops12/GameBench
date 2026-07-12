#include "Enemy.hxx"

#include <raylib.h>

#include <algorithm>
#include <cmath>

namespace {
	constexpr Color OUTLINE{226, 230, 211, 255};
	constexpr Color HUNTER{190, 74, 86, 255};
	constexpr Color TORCH{224, 154, 90, 255};
	constexpr Color SUPPRESSOR{102, 206, 221, 255};
	constexpr Color BODY{25, 29, 38, 255};
	constexpr Color HIT_RED{224, 80, 92, 255};
	constexpr Color SNARE_GREEN{114, 214, 128, 255};

	bool blocked(const BoundingBox& box, Vector3 candidate, float radius) {
		const float closestX = std::clamp(candidate.x, box.min.x, box.max.x);
		const float closestZ = std::clamp(candidate.z, box.min.z, box.max.z);
		const float dx = candidate.x - closestX;
		const float dz = candidate.z - closestZ;
		return dx * dx + dz * dz < radius * radius && candidate.y < box.max.y;
	}
}

Enemy::Enemy(EnemyKind type, Vector3 initialPosition)
	: Actor(initialPosition), enemyKind(type) {
	switch (enemyKind) {
		case EnemyKind::TorchBearer:
			maximumHealth = 42.0f;
			radius = 0.42f;
			break;
		case EnemyKind::WitchHunter:
			maximumHealth = 68.0f;
			radius = 0.48f;
			break;
		case EnemyKind::Suppressor:
			maximumHealth = 92.0f;
			radius = 0.55f;
			break;
	}
	currentHealth = maximumHealth;
}

bool Enemy::updateTowardsPlayer(float deltaTime, Vector3 playerPosition, const std::vector<BoundingBox>& obstacles) {
	if (!isAlive()) return false;
	attackTimer = std::max(0.0f, attackTimer - deltaTime);
	snareTimer = std::max(0.0f, snareTimer - deltaTime);
	fearTimer = std::max(0.0f, fearTimer - deltaTime);
	hitFlash = std::max(0.0f, hitFlash - deltaTime);
	bobTime += deltaTime;

	if (poisonTimer > 0.0f) {
		poisonTimer = std::max(0.0f, poisonTimer - deltaTime);
		takeDamage(poisonDamage * deltaTime);
		if (!isAlive()) return false;
	}

	Vector3 direction{playerPosition.x - position.x, 0.0f, playerPosition.z - position.z};
	float distance = std::sqrt(direction.x * direction.x + direction.z * direction.z);
	if (distance > 0.001f) {
		direction.x /= distance;
		direction.z /= distance;
	}

	const float attackDistance = enemyKind == EnemyKind::Suppressor ? 2.0f : 1.5f;
	if (distance > attackDistance * 0.92f || fearTimer > 0.0f) {
		const float speed = (enemyKind == EnemyKind::Suppressor ? 1.25f : (enemyKind == EnemyKind::WitchHunter ? 1.8f : 2.15f)) * (snareTimer > 0.0f ? 0.35f : 1.0f);
		if (fearTimer > 0.0f) {
			direction.x = -direction.x;
			direction.z = -direction.z;
		}
		Vector3 candidate = position;
		candidate.x += direction.x * speed * deltaTime;
		candidate.z += direction.z * speed * deltaTime;
		for (const BoundingBox& obstacle : obstacles) {
			if (blocked(obstacle, candidate, radius)) {
				candidate = position;
				break;
			}
		}
		position.x = std::clamp(candidate.x, -14.0f, 14.0f);
		position.z = std::clamp(candidate.z, -11.0f, 11.0f);
	}

	if (fearTimer <= 0.0f && distance <= attackDistance && attackTimer <= 0.0f) {
		attackTimer = enemyKind == EnemyKind::Suppressor ? 1.8f : 1.15f;
		return true;
	}
	return false;
}

void Enemy::applySpell(const SpellStats& stats) {
	if (!isAlive()) return;
	if (stats.damage > 0) takeDamage(static_cast<float>(stats.damage));
	poisonDamage = std::max(poisonDamage, stats.poisonPerSecond);
	poisonTimer = std::max(poisonTimer, stats.poisonPerSecond > 0.0f ? 3.0f : 0.0f);
	snareTimer = std::max(snareTimer, stats.snareDuration);
	fearTimer = std::max(fearTimer, stats.fearDuration);
	hitFlash = 0.2f;
}

void Enemy::draw() const {
	if (!isAlive()) return;
	const Color accent = accentColor();
	const float bob = std::sin(bobTime * 4.0f + position.x) * 0.035f;
	const float bodyHeight = enemyKind == EnemyKind::Suppressor ? 1.65f : 1.45f;
	const float bodyWidth = enemyKind == EnemyKind::Suppressor ? 0.82f : 0.68f;
	const Color bodyColor = hitFlash > 0.0f ? Color{245, 87, 90, 255} : BODY;
	const Vector3 body{position.x, position.y + bodyHeight * 0.5f + bob, position.z};

	DrawCubeV(body, {bodyWidth, bodyHeight, bodyWidth * 0.55f}, bodyColor);
	DrawCubeWiresV(body, {bodyWidth, bodyHeight, bodyWidth * 0.55f}, OUTLINE);
	DrawSphere({position.x, position.y + bodyHeight + 0.2f + bob, position.z}, 0.25f, bodyColor);
	DrawSphereWires({position.x, position.y + bodyHeight + 0.2f + bob, position.z}, 0.27f, 8, 6, OUTLINE);
	DrawCylinder({position.x, position.y + bodyHeight + 0.38f + bob, position.z}, 0.43f, 0.35f, 0.16f, 8, bodyColor);
	DrawCylinderWires({position.x, position.y + bodyHeight + 0.38f + bob, position.z}, 0.43f, 0.35f, 0.16f, 8, accent);

	DrawLine3D({position.x - bodyWidth * 0.3f, position.y + bodyHeight * 0.62f, position.z - bodyWidth * 0.35f}, {position.x + bodyWidth * 0.3f, position.y + bodyHeight * 0.62f, position.z - bodyWidth * 0.35f}, accent);
	DrawLine3D({position.x, position.y + 0.05f, position.z - bodyWidth * 0.2f}, {position.x, position.y + bodyHeight, position.z - bodyWidth * 0.2f}, ColorAlpha(accent, 0.7f));

	if (enemyKind == EnemyKind::TorchBearer) {
		const Vector3 torchBottom{position.x - 0.53f, position.y + 0.55f, position.z};
		const Vector3 torchTop{position.x - 0.53f, position.y + 1.4f, position.z};
		DrawLine3D(torchBottom, torchTop, OUTLINE);
		DrawSphere(torchTop, 0.16f, ColorAlpha(TORCH, 0.9f));
		DrawSphereWires(torchTop, 0.23f, 8, 6, TORCH);
	} else if (enemyKind == EnemyKind::WitchHunter) {
		DrawLine3D({position.x + 0.48f, position.y + 0.3f, position.z}, {position.x + 0.48f, position.y + 1.2f, position.z}, accent);
		DrawCircle3D({position.x + 0.48f, position.y + 1.25f, position.z}, 0.22f, {0.0f, 0.0f, 1.0f}, 0.0f, accent);
	} else {
		DrawCircle3D({position.x, position.y + 0.8f, position.z - 0.35f}, 0.52f, {0.0f, 1.0f, 0.0f}, 90.0f, ColorAlpha(SUPPRESSOR, 0.5f));
		DrawLine3D({position.x - 0.6f, position.y + 0.8f, position.z}, {position.x + 0.6f, position.y + 0.8f, position.z}, SUPPRESSOR);
	}

	if (snareTimer > 0.0f) {
			DrawCircle3D({position.x, 0.04f, position.z}, 0.55f, {1.0f, 0.0f, 0.0f}, 90.0f, SNARE_GREEN);
	}
	if (fearTimer > 0.0f) {
		DrawLine3D({position.x - 0.35f, position.y + 1.9f, position.z}, {position.x + 0.35f, position.y + 2.25f, position.z}, HIT_RED);
		DrawLine3D({position.x + 0.35f, position.y + 1.9f, position.z}, {position.x - 0.35f, position.y + 2.25f, position.z}, HIT_RED);
	}
}

EnemyKind Enemy::kind() const {
	return enemyKind;
}

float Enemy::attackDamage() const {
	switch (enemyKind) {
		case EnemyKind::TorchBearer: return 7.0f;
		case EnemyKind::WitchHunter: return 11.0f;
		case EnemyKind::Suppressor: return 15.0f;
	}
	return 7.0f;
}

float Enemy::snareTime() const {
	return snareTimer;
}

float Enemy::fearTime() const {
	return fearTimer;
}

float Enemy::poisonTime() const {
	return poisonTimer;
}

bool Enemy::recentlyHit() const {
	return hitFlash > 0.0f;
}

Color Enemy::accentColor() const {
	switch (enemyKind) {
		case EnemyKind::TorchBearer: return TORCH;
		case EnemyKind::WitchHunter: return HUNTER;
		case EnemyKind::Suppressor: return SUPPRESSOR;
	}
	return OUTLINE;
}
