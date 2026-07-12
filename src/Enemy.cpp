#include "Enemy.hxx"

#include <algorithm>
#include <cmath>

#include <raymath.h>

#include "CurseVector.hxx"
#include "Player.hxx"

Enemy::Enemy(Vector3 initialPosition) : Actor(initialPosition, 36.0f) {
	collisionRadius = 0.48f;
	int drop = static_cast<int>(std::abs(initialPosition.x * 3.0f + initialPosition.z * 5.0f));
	dropType = static_cast<ReagentType>(drop % REAGENT_COUNT);
}

void Enemy::update(Player& player, float deltaTime) {
	if (isDead()) return;
	poisonTime = std::max(0.0f, poisonTime - deltaTime);
	if (poisonTime > 0.0f) takeDamage(poisonDamagePerSecond * deltaTime);
	if (isDead()) return;

	Vector3 toPlayer = Vector3Subtract(player.getPosition(), position);
	toPlayer.y = 0.0f;
	float distance = Vector3Length(toPlayer);
	if (distance > 0.01f) {
		Vector3 direction = Vector3Scale(toPlayer, 1.0f / distance);
		fearTime = std::max(0.0f, fearTime - deltaTime);
		if (fearTime > 0.0f) direction = Vector3Negate(direction);
		float speed = fearTime > 0.0f ? 4.2f : 2.25f;
		if (snareTime > 0.0f) {
			snareTime = std::max(0.0f, snareTime - deltaTime);
			speed *= 0.3f;
		}
		if (distance > 1.25f || fearTime > 0.0f) position = Vector3Add(position, Vector3Scale(direction, speed * deltaTime));
	}

	attackTimer = std::max(0.0f, attackTimer - deltaTime);
	if (distance < 1.55f && fearTime <= 0.0f && attackTimer <= 0.0f) {
		player.takeDamage(8.0f);
		attackTimer = 1.1f;
	}
}

void Enemy::applyCurse(const SpellStats& spell) {
	takeDamage(spell.damage);
	poisonDamagePerSecond = std::max(poisonDamagePerSecond, spell.poisonDamagePerSecond);
	poisonTime = std::max(poisonTime, spell.poisonDuration);
	snareTime = std::max(snareTime, spell.snareDuration);
	fearTime = std::max(fearTime, spell.fearDuration);
}

void Enemy::draw3D() const {
	if (!active || isDead()) return;
	Color body = fearTime > 0.0f ? Color {174, 132, 239, 255} : Color {229, 91, 108, 255};
	if (snareTime > 0.0f) body = {99, 214, 115, 255};
	DrawCylinderWires({position.x, position.y + 0.72f, position.z}, 0.30f, 0.43f, 1.25f, 6, body);
	DrawSphereWires({position.x, position.y + 1.52f, position.z}, 0.24f, 6, 4, RAYWHITE);
	DrawLine3D({position.x - 0.43f, position.y + 1.05f, position.z}, {position.x + 0.43f, position.y + 0.95f, position.z}, body);
	float ratio = health / maxHealth;
	DrawCube({position.x, position.y + 1.96f, position.z}, 0.72f, 0.055f, 0.04f, Fade(BLACK, 0.8f));
	DrawCube({position.x - (0.72f * (1.0f - ratio) * 0.5f), position.y + 1.96f, position.z - 0.01f}, 0.72f * ratio, 0.06f, 0.045f, body);
}

bool Enemy::canDropPickup() const { return isDead() && active; }
ReagentType Enemy::getDropType() const { return dropType; }
