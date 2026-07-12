#include "Enemy.hxx"

#include <algorithm>

#include <raymath.h>

namespace {

float healthFor(EnemyType type) {
	switch (type) {
	case EnemyType::TorchBearer:
		return 28.0f;
	case EnemyType::WitchHunter:
		return 42.0f;
	case EnemyType::Hound:
		return 18.0f;
	}
	return 28.0f;
}

Color colorFor(EnemyType type) {
	switch (type) {
	case EnemyType::TorchBearer:
		return {255, 156, 83, 255};
	case EnemyType::WitchHunter:
		return {255, 90, 98, 255};
	case EnemyType::Hound:
		return {197, 139, 255, 255};
	}
	return RAYWHITE;
}

}

Enemy::Enemy(EnemyType initialType, Vector3 initialPosition)
	: Actor(initialPosition, initialType == EnemyType::Hound ? 0.42f : 0.52f, healthFor(initialType)), type(initialType) {
	switch (type) {
	case EnemyType::TorchBearer:
		movementSpeed = 1.85f;
		attackRange = 1.35f;
		attackDamage = 5.0f;
		break;
	case EnemyType::WitchHunter:
		movementSpeed = 1.6f;
		attackRange = 1.65f;
		attackDamage = 7.0f;
		break;
	case EnemyType::Hound:
		movementSpeed = 2.75f;
		attackRange = 1.05f;
		attackDamage = 4.0f;
		break;
	}
}

void Enemy::update(float deltaTime, Vector3 playerPosition) {
	if (!isAlive()) {
		return;
	}

	hitFlash = std::max(0.0f, hitFlash - deltaTime);
	attackTimer = std::max(0.0f, attackTimer - deltaTime);
	snareRemaining = std::max(0.0f, snareRemaining - deltaTime);
	fearRemaining = std::max(0.0f, fearRemaining - deltaTime);

	if (rotRemaining > 0.0f) {
		rotRemaining = std::max(0.0f, rotRemaining - deltaTime);
		rotTimer -= deltaTime;
		while (rotTimer <= 0.0f && rotRemaining > 0.0f) {
			takeDamage(rotDamagePerSecond * 0.25f);
			rotTimer += 0.25f;
		}
	}

	if (!isAlive()) {
		return;
	}

	Vector3 towardPlayer = Vector3Subtract(playerPosition, position);
	towardPlayer.y = 0.0f;
	const float distance = Vector3Length(towardPlayer);
	if (distance <= 0.01f) {
		return;
	}

	const Vector3 direction = Vector3Scale(towardPlayer, 1.0f / distance);
	const float movementScale = snareRemaining > 0.0f ? 0.22f : 1.0f;
	if (fearRemaining > 0.0f) {
		position = Vector3Subtract(position, Vector3Scale(direction, movementSpeed * movementScale * deltaTime));
	} else if (distance > attackRange) {
		position = Vector3Add(position, Vector3Scale(direction, movementSpeed * movementScale * deltaTime));
	} else if (attackTimer <= 0.0f) {
		pendingDamage += attackDamage;
		attackTimer = type == EnemyType::WitchHunter ? 1.05f : 0.82f;
	}
}

void Enemy::draw3D() const {
	if (!isAlive() || !visible) {
		return;
	}

	Color lineColor = colorFor(type);
	if (hitFlash > 0.0f) {
		lineColor = RAYWHITE;
	}
	if (fearRemaining > 0.0f) {
		lineColor = {130, 219, 255, 255};
	}
	if (snareRemaining > 0.0f) {
		lineColor = {111, 246, 139, 255};
	}

	if (type == EnemyType::Hound) {
		const Vector3 body {position.x, 0.45f, position.z};
		DrawCube(body, 0.9f, 0.52f, 0.62f, Fade({24, 18, 34, 255}, 0.92f));
		DrawCubeWires(body, 0.9f, 0.52f, 0.62f, lineColor);
		DrawSphere({position.x, 0.58f, position.z - 0.42f}, 0.26f, Fade(lineColor, 0.36f));
		DrawSphereWires({position.x, 0.58f, position.z - 0.42f}, 0.28f, 6, 4, lineColor);
		for (float x : {-0.3f, 0.3f}) {
			DrawLine3D({position.x + x, 0.3f, position.z - 0.2f}, {position.x + x, 0.02f, position.z - 0.2f}, lineColor);
			DrawLine3D({position.x + x, 0.3f, position.z + 0.2f}, {position.x + x, 0.02f, position.z + 0.2f}, lineColor);
		}
	} else {
		const Vector3 body {position.x, 0.86f, position.z};
		const Vector3 head {position.x, 1.62f, position.z};
		DrawCylinder(body, 0.24f, 0.36f, 1.18f, 6, Fade({21, 27, 38, 255}, 0.94f));
		DrawCylinderWires(body, 0.24f, 0.36f, 1.18f, 6, lineColor);
		DrawSphere(head, 0.27f, Fade(lineColor, 0.25f));
		DrawSphereWires(head, 0.29f, 7, 5, RAYWHITE);
		DrawLine3D({position.x - 0.42f, 1.14f, position.z}, {position.x + 0.42f, 1.14f, position.z}, lineColor);
		if (type == EnemyType::TorchBearer) {
			DrawLine3D({position.x + 0.42f, 1.12f, position.z}, {position.x + 0.65f, 1.73f, position.z}, lineColor);
			DrawSphere({position.x + 0.65f, 1.8f, position.z}, 0.12f, {255, 186, 75, 255});
		} else {
			DrawLine3D({position.x - 0.45f, 1.04f, position.z}, {position.x - 0.72f, 0.55f, position.z}, lineColor);
		}
	}

	const float healthWidth = 0.72f;
	const float filled = healthWidth * getHealthRatio();
	DrawLine3D({position.x - healthWidth * 0.5f, 2.2f, position.z}, {position.x + healthWidth * 0.5f, 2.2f, position.z}, {67, 74, 94, 255});
	DrawLine3D({position.x - healthWidth * 0.5f, 2.2f, position.z - 0.015f}, {position.x - healthWidth * 0.5f + filled, 2.2f, position.z - 0.015f}, lineColor);

	if (fearRemaining > 0.0f) {
		DrawCircle3D({position.x, 0.045f, position.z}, 0.72f, {1.0f, 0.0f, 0.0f}, 90.0f, Fade(lineColor, 0.7f));
	}
	if (snareRemaining > 0.0f) {
		DrawCircle3D({position.x, 0.05f, position.z}, 0.42f, {1.0f, 0.0f, 0.0f}, 90.0f, Fade(lineColor, 0.8f));
	}
}

void Enemy::applySpell(const SpellStats& spell, float damageScale) {
	if (!isAlive()) {
		return;
	}

	takeDamage(spell.damage * damageScale);
	rotRemaining = std::max(rotRemaining, spell.rotDuration);
	rotDamagePerSecond = std::max(rotDamagePerSecond, spell.rotDamagePerSecond);
	rotTimer = std::min(rotTimer, 0.15f);
	snareRemaining = std::max(snareRemaining, spell.snareDuration);
	fearRemaining = std::max(fearRemaining, spell.fearDuration);
	hitFlash = 0.13f;
}

float Enemy::consumePendingDamage() {
	const float result = pendingDamage;
	pendingDamage = 0.0f;
	return result;
}

bool Enemy::justDied() {
	return !isAlive() && !deathHandled;
}

bool Enemy::hasDiedHandled() const {
	return deathHandled;
}

void Enemy::markDeathHandled() {
	deathHandled = true;
}

EnemyType Enemy::getType() const {
	return type;
}

const char* Enemy::getName() const {
	switch (type) {
	case EnemyType::TorchBearer:
		return "Torch Bearer";
	case EnemyType::WitchHunter:
		return "Witch Hunter";
	case EnemyType::Hound:
		return "Hunting Hound";
	}
	return "Hunter";
}

float Enemy::getFearRemaining() const {
	return fearRemaining;
}

float Enemy::getSnareRemaining() const {
	return snareRemaining;
}
