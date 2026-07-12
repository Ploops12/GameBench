#include "Enemy.hxx"

#include <algorithm>
#include <cmath>

#include <raymath.h>

#include "Player.hxx"

namespace {
Color enemyColor(EnemyArchetype archetype) {
	return archetype == EnemyArchetype::TorchBearer
		? Color {255, 181, 93, 255}
		: Color {245, 109, 126, 255};
}

}

Enemy::Enemy(Vector3 initialPosition, EnemyArchetype initialArchetype, ReagentType initialDropType)
	: Actor(initialPosition, 0.48f, initialArchetype == EnemyArchetype::TorchBearer ? 32.0f : 46.0f),
	  archetype(initialArchetype), dropType(initialDropType),
	  speed(initialArchetype == EnemyArchetype::TorchBearer ? 2.3f : 1.8f) {
}

void Enemy::update(float deltaTime, Player& player, const std::vector<BoundingBox>& solidBounds) {
	if (!isAlive() || !player.isAlive()) {
		return;
	}

	attackCooldown = std::max(0.0f, attackCooldown - deltaTime);
	snareTimer = std::max(0.0f, snareTimer - deltaTime);
	fearTimer = std::max(0.0f, fearTimer - deltaTime);
	hurtTimer = std::max(0.0f, hurtTimer - deltaTime);

	if (poisonTimer > 0.0f) {
		const bool wasAlive = isAlive();
		const float activeTime = std::min(poisonTimer, deltaTime);
		poisonTimer = std::max(0.0f, poisonTimer - activeTime);
		takeDamage(poisonDamagePerSecond * activeTime);
		markDeathIfNeeded(wasAlive);
		if (poisonTimer <= 0.0f) {
			poisonDamagePerSecond = 0.0f;
		}
		if (!isAlive()) {
			return;
		}
	}

	Vector3 toPlayer = Vector3Subtract(player.getPosition(), position);
	toPlayer.y = 0.0f;
	const float distance = Vector3Length(toPlayer);
	if (distance <= 0.001f) {
		return;
	}

	Vector3 direction = Vector3Scale(toPlayer, 1.0f / distance);
	if (fearTimer > 0.0f) {
		direction = Vector3Negate(direction);
	}

	if (distance > 1.12f || fearTimer > 0.0f) {
		const float snareMultiplier = snareTimer > 0.0f ? 0.32f : 1.0f;
		Vector3 candidate = position;
		candidate.x += direction.x * speed * snareMultiplier * deltaTime;
		candidate.z += direction.z * speed * snareMultiplier * deltaTime;
		if (!collidesWithWorld(candidate, solidBounds)) {
			position = candidate;
		}
	} else if (attackCooldown <= 0.0f) {
		const float thornDamage = player.getThornRetaliationDamage();
		const float thornSnare = player.getThornSnareDuration();
		player.takeDamage(archetype == EnemyArchetype::TorchBearer ? 7.0f : 10.0f);
		if (thornDamage > 0.0f && isAlive()) {
			applyThornRetaliation(thornDamage, thornSnare);
		}
		attackCooldown = archetype == EnemyArchetype::TorchBearer ? 1.05f : 0.82f;
	}
}

void Enemy::draw(const Camera3D& camera) const {
	draw(camera, nullptr);
}

void Enemy::draw(const Camera3D& camera, const Texture2D* billboardTexture) const {
	if (!isAlive() || !isDrawable()) {
		return;
	}

	const Color baseColor = hurtTimer > 0.0f ? RAYWHITE : enemyColor(archetype);
	const Vector3 billboardPosition {position.x, position.y + 1.16f, position.z};
	if (billboardTexture != nullptr && billboardTexture->id != 0) {
		const float frameWidth = static_cast<float>(billboardTexture->width) / 8.0f;
		const float frameHeight = static_cast<float>(billboardTexture->height) / 3.0f;
		const float animationTime = static_cast<float>(GetTime());
		const int frame = static_cast<int>(fmodf(animationTime * 5.0f, 8.0f));
		const Rectangle source {frameWidth * static_cast<float>(frame), 0.0f, frameWidth, frameHeight};
		DrawBillboardRec(camera, *billboardTexture, source, billboardPosition,
			Vector2 {1.45f, 2.45f}, baseColor);
	} else {
		DrawCubeWires(billboardPosition, 0.84f, 2.1f, 0.12f, baseColor);
		DrawLine3D(Vector3 {position.x - 0.5f, position.y + 1.3f, position.z},
			Vector3 {position.x + 0.5f, position.y + 1.3f, position.z}, baseColor);
		DrawSphereWires(Vector3 {position.x, position.y + 2.25f, position.z}, 0.24f, 6, 6, baseColor);
	}

	const float healthFraction = getHealth() / getMaximumHealth();
	const Vector3 barBase {position.x - 0.46f, position.y + 2.55f, position.z};
	DrawLine3D(barBase, Vector3 {position.x + 0.46f, barBase.y, barBase.z}, Color {38, 44, 56, 255});
	DrawLine3D(barBase, Vector3 {position.x - 0.46f + 0.92f * healthFraction, barBase.y, barBase.z}, baseColor);
	if (snareTimer > 0.0f) {
		DrawCircle3D(Vector3 {position.x, 0.04f, position.z}, 0.58f, Vector3 {1.0f, 0.0f, 0.0f}, 90.0f, Color {121, 243, 109, 190});
	}
	if (fearTimer > 0.0f) {
		DrawCircle3D(Vector3 {position.x, position.y + 2.7f, position.z}, 0.25f, Vector3 {1.0f, 0.0f, 0.0f}, 90.0f, Color {194, 125, 230, 220});
	}
}

void Enemy::applyHex(const SpellStats& spell, float strength) {
	if (!isAlive() || strength <= 0.0f) {
		return;
	}

	const bool wasAlive = isAlive();
	if (spell.damage > 0.0f) {
		takeDamage(spell.damage * strength);
	}
	markDeathIfNeeded(wasAlive);
	if (!isAlive()) {
		return;
	}

	snareTimer = std::max(snareTimer, spell.snareDuration * strength);
	fearTimer = std::max(fearTimer, spell.fearDuration * strength);
	if (spell.poisonDuration > 0.0f) {
		poisonTimer = std::max(poisonTimer, spell.poisonDuration * strength);
		poisonDamagePerSecond = 2.5f + spell.damage * 0.18f;
	}
	hurtTimer = 0.18f;
}

void Enemy::applyThornRetaliation(float damage, float snareDuration) {
	if (!isAlive() || damage <= 0.0f) {
		return;
	}

	const bool wasAlive = isAlive();
	takeDamage(damage);
	markDeathIfNeeded(wasAlive);
	if (isAlive()) {
		snareTimer = std::max(snareTimer, snareDuration);
		hurtTimer = 0.18f;
	}
}

bool Enemy::hasDrop() const {
	return dropAvailable;
}

bool Enemy::takeDrop(ReagentType& result) {
	if (!dropAvailable) {
		return false;
	}

	dropAvailable = false;
	result = dropType;
	return true;
}

EnemyArchetype Enemy::getArchetype() const {
	return archetype;
}

Vector3 Enemy::getTargetPosition() const {
	return Vector3 {position.x, position.y + 1.12f, position.z};
}

float Enemy::getFearTimer() const {
	return fearTimer;
}

float Enemy::getSnareTimer() const {
	return snareTimer;
}

bool Enemy::collidesWithWorld(Vector3 candidate, const std::vector<BoundingBox>& solidBounds) const {
	const Vector3 centre {candidate.x, 0.8f, candidate.z};
	for (const BoundingBox& bounds : solidBounds) {
		if (CheckCollisionBoxSphere(bounds, centre, collisionRadius)) {
			return true;
		}
	}

	return false;
}

void Enemy::markDeathIfNeeded(bool wasAlive) {
	if (wasAlive && !isAlive()) {
		dropAvailable = true;
	}
}
