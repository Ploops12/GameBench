#include "Enemy.hxx"

#include <raymath.h>

#include <cmath>

Enemy::Enemy(Vector3 start, int variantValue)
	: variant(variantValue) {
	position = start;
	maxHealth = health = 42.0f + variant * 14.0f;
	radius = 0.62f;
}

void Enemy::Update(float dt, const Vector3& target, bool targetVulnerable) {
	attackTimer -= dt;
	snareSeconds = std::fmax(0.0f, snareSeconds - dt);
	fearSeconds = std::fmax(0.0f, fearSeconds - dt);
	if (poisonTimer > 0.0f) {
		poisonTimer -= dt;
		TakeDamage(poisonDamage * dt);
	}
	if (IsDead()) return;
	Vector3 toTarget = Vector3Subtract(target, position);
	toTarget.y = 0.0f;
	const float distance = Vector3Length(toTarget);
	if (distance < 0.01f) return;
	Vector3 direction = Vector3Normalize(toTarget);
	if (fearSeconds > 0.0f) direction = Vector3Scale(direction, -1.0f);
	const float speed = snareSeconds > 0.0f ? 0.35f : 1.9f + variant * 0.25f;
	if (distance > 1.65f || fearSeconds > 0.0f) {
		position = Vector3Add(position, Vector3Scale(direction, speed * dt));
	}
	if (targetVulnerable && distance < 2.0f && fearSeconds <= 0.0f) {
		// Game owns the actual damage call; this keeps enemy attack timing in the enemy model.
	}
}

void Enemy::Draw(const Camera3D& camera, Texture2D spriteSheet) const {
	if (IsDead()) return;
	if (spriteSheet.id != 0) {
		const Rectangle frame{0.0f, 0.0f, static_cast<float>(spriteSheet.width) / 8.0f, static_cast<float>(spriteSheet.height) / 3.0f};
		const Color tint = variant == 0 ? WHITE : (variant == 1 ? Color{255, 210, 150, 255} : Color{190, 210, 255, 255});
		DrawBillboardRec(camera, spriteSheet, frame, {position.x, position.y + 1.0f, position.z}, {2.5f, 2.5f}, tint);
	} else {
		DrawCube(position, 1.0f, 1.9f, 0.7f, MAROON);
		DrawCubeWires(position, 1.0f, 1.9f, 0.7f, RED);
	}
	const float healthWidth = 1.0f * (health / maxHealth);
	DrawCube({position.x, position.y + 2.45f, position.z}, healthWidth, 0.07f, 0.04f, LIME);
}

bool Enemy::TakeDamage(float amount) {
	if (IsDead()) return true;
	health -= amount;
	return IsDead();
}

void Enemy::AddStatus(float snare, float fear, float poison, float duration) {
	snareSeconds = std::fmax(snareSeconds, snare);
	fearSeconds = std::fmax(fearSeconds, fear);
	if (poison > 0.0f) {
		poisonDamage = std::fmax(poisonDamage, poison);
		poisonTimer = std::fmax(poisonTimer, duration);
	}
}
