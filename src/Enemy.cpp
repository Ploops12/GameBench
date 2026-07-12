#include <algorithm>
#include <cmath>
#include "Enemy.hxx"
#include "Player.hxx"
#include <raymath.h>

Enemy::Enemy(Vector3 position)
	: Actor(position, 0.48f, 42.0f) {
}

void Enemy::setSprite(Sprite* sprite) {
	animation.setSprite(sprite);
	animation.setGrid(8, 3);
	animation.setPlayback(10.0f, 0, 8, 0);
}

void Enemy::updateAI(float dt, const Player& player, const std::vector<BoundingBox>& blockers, const std::vector<Vector3>& neighbors) {
	if (!isAlive()) {
		return;
	}

	flashTime = std::max(0.0f, flashTime - dt);
	rotTimeRemaining = std::max(0.0f, rotTimeRemaining - dt);
	snareTimeRemaining = std::max(0.0f, snareTimeRemaining - dt);
	fearTimeRemaining = std::max(0.0f, fearTimeRemaining - dt);
	attackCooldown = std::max(0.0f, attackCooldown - dt);

	if (rotTimeRemaining > 0.0f) {
		takeDamage(rotDamagePerSecond * dt);
	}

	Vector3 toPlayer = Vector3Subtract(player.getPosition(), position);
	toPlayer.y = 0.0f;
	const float distance = Vector3Length(toPlayer);
	Vector3 move = distance > 0.01f ? Vector3Scale(Vector3Normalize(toPlayer), 1.0f) : Vector3 {};
	if (fearTimeRemaining > 0.0f) {
		move = Vector3Scale(move, -1.0f);
	}

	for (const Vector3& other : neighbors) {
		if (Vector3Equals(other, position)) {
			continue;
		}
		Vector3 separation = Vector3Subtract(position, other);
		separation.y = 0.0f;
		const float separationDistance = Vector3Length(separation);
		if (separationDistance > 0.001f && separationDistance < 1.2f) {
			move = Vector3Add(move, Vector3Scale(Vector3Normalize(separation), (1.2f - separationDistance) * 0.9f));
		}
	}

	if (Vector3Length(move) > 0.01f) {
		move = Vector3Normalize(move);
	}

	float speed = snareTimeRemaining > 0.0f ? 1.0f : 2.7f;
	if (distance < 1.25f && fearTimeRemaining <= 0.0f) {
		speed = 0.0f;
	}
	velocity.x = move.x * speed;
	velocity.z = move.z * speed;
	position.x += velocity.x * dt;
	position.z += velocity.z * dt;
	resolveCollisions(blockers);
	position.x = std::clamp(position.x, -23.0f, 23.0f);
	position.z = std::clamp(position.z, -34.0f, 34.0f);

	animation.update(dt);
}

void Enemy::draw3D(const Camera3D& camera) const {
	if (!isAlive()) {
		return;
	}

	const Color tint = flashTime > 0.0f ? Color {255, 124, 124, 255} : WHITE;
	if (animation.isReady()) {
		Rectangle source = animation.currentFrame();
		DrawBillboardRec(camera, animation.texture(), source, Vector3 {position.x, 1.55f, position.z}, Vector2 {2.2f, 3.0f}, tint);
	} else {
		DrawCube(position, 0.8f, 2.2f, 0.8f, Color {35, 40, 48, 255});
		DrawCubeWires(position, 0.8f, 2.2f, 0.8f, Color {180, 210, 230, 200});
	}

	DrawSphereWires(Vector3 {position.x, 0.2f, position.z}, 0.22f, 8, 8, Color {255, 90, 90, 90});
}

void Enemy::applySpell(const SpellPreview& spell, const Vector3& source) {
	if (!isAlive()) {
		return;
	}

	takeDamage(spell.damage);
	if (spell.rotDamagePerSecond > 0.0f) {
		rotDamagePerSecond = std::max(rotDamagePerSecond, spell.rotDamagePerSecond);
		rotTimeRemaining = std::max(rotTimeRemaining, spell.rotDuration);
	}
	if (spell.snareDuration > 0.0f) {
		snareTimeRemaining = std::max(snareTimeRemaining, spell.snareDuration);
	}
	if (spell.fearDuration > 0.0f) {
		fearTimeRemaining = std::max(fearTimeRemaining, spell.fearDuration);
	}

	Vector3 away = Vector3Subtract(position, source);
	away.y = 0.0f;
	if (Vector3Length(away) > 0.01f) {
		away = Vector3Normalize(away);
		position = Vector3Add(position, Vector3Scale(away, 0.3f));
	}
	flashTime = 0.14f;
}

void Enemy::applyKnockback(const Vector3& impulse) {
	position = Vector3Add(position, impulse);
}

bool Enemy::tryAttack(float, Player& player) {
	if (!isAlive() || attackCooldown > 0.0f) {
		return false;
	}

	const float distance = Vector3Distance(position, player.getPosition());
	if (distance > 1.35f || fearTimeRemaining > 0.0f) {
		return false;
	}

	player.applyIncomingDamage(11.0f);
	attackCooldown = 0.95f;
	return true;
}

bool Enemy::isRecentlyHit() const {
	return flashTime > 0.0f;
}

bool Enemy::shouldDropLoot() const {
	return !isAlive() && !lootDropped;
}

void Enemy::markLootDropped() {
	lootDropped = true;
}

void Enemy::resolveCollisions(const std::vector<BoundingBox>& blockers) {
	for (const BoundingBox& box : blockers) {
		const float closestX = std::clamp(position.x, box.min.x, box.max.x);
		const float closestZ = std::clamp(position.z, box.min.z, box.max.z);
		const float dx = position.x - closestX;
		const float dz = position.z - closestZ;
		const float distanceSquared = dx * dx + dz * dz;
		const float radiusSquared = collisionRadius * collisionRadius;
		if (distanceSquared >= radiusSquared) {
			continue;
		}

		float distance = std::sqrt(std::max(distanceSquared, 0.0001f));
		float push = collisionRadius - distance;
		Vector3 normal {dx / distance, 0.0f, dz / distance};
		position = Vector3Add(position, Vector3Scale(normal, push));
	}
}
