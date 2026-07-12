#include "Enemy.hxx"

#include <raymath.h>

#include <algorithm>
#include <cmath>

namespace {
Vector3 BillboardPoint(Vector3 origin, Vector3 right, float horizontal, float vertical) {
	return {
		origin.x + right.x * horizontal,
		origin.y + vertical,
		origin.z + right.z * horizontal
	};
}

bool HasLineOfSight(Vector3 start, Vector3 end, const std::vector<BoundingBox>& obstacles) {
	const Vector3 offset = Vector3Subtract(end, start);
	const float distance = Vector3Length(offset);
	if (distance <= 0.001f) {
		return true;
	}
	const Ray ray {start, Vector3Scale(offset, 1.0f / distance)};
	for (const BoundingBox obstacle : obstacles) {
		const RayCollision collision = GetRayCollisionBox(ray, obstacle);
		if (collision.hit && collision.distance < distance - 0.05f) {
			return false;
		}
	}
	return true;
}
}

Enemy::Enemy(Vector3 position, EnemyArchetype archetype, int threatLevel, ReagentType dropType)
	: Actor(position,
		archetype == EnemyArchetype::TorchBearer ? 48.0f + threatLevel * 7.0f : 72.0f + threatLevel * 10.0f,
		0.42f, archetype == EnemyArchetype::TorchBearer ? 1.82f : 1.92f),
	  archetype_(archetype), dropType_(dropType) {
	moveSpeed_ = (archetype == EnemyArchetype::TorchBearer ? 2.35f : 1.85f) + threatLevel * 0.12f;
	attackDamage_ = (archetype == EnemyArchetype::TorchBearer ? 8.0f : 13.0f) + threatLevel * 1.5f;
	attackCooldown_ = 0.35f + static_cast<float>(GetRandomValue(0, 60)) / 100.0f;
}

float Enemy::updateCombat(float dt, Vector3 playerPosition, const std::vector<BoundingBox>& obstacles) {
	attackCooldown_ -= dt;
	hitFlash_ = std::max(0.0f, hitFlash_ - dt * 5.0f);
	attackFlash_ = std::max(0.0f, attackFlash_ - dt * 4.0f);
	walkPhase_ += dt * 7.0f;

	if (poisonTimer_ > 0.0f && isAlive()) {
		Actor::takeDamage(poisonDps_ * dt);
		poisonTimer_ = std::max(0.0f, poisonTimer_ - dt);
		if (poisonTimer_ <= 0.0f) {
			poisonDps_ = 0.0f;
		}
	}
	snareTimer_ = std::max(0.0f, snareTimer_ - dt);
	fearTimer_ = std::max(0.0f, fearTimer_ - dt);
	if (!isAlive()) {
		return 0.0f;
	}

	Vector3 towardPlayer = Vector3Subtract(playerPosition, position_);
	towardPlayer.y = 0.0f;
	const float distance = Vector3Length(towardPlayer);
	if (distance > 0.001f) {
		towardPlayer = Vector3Scale(towardPlayer, 1.0f / distance);
	}
	Vector3 moveDirection = fearTimer_ > 0.0f ? Vector3Negate(towardPlayer) : towardPlayer;
	const float snareMultiplier = snareTimer_ > 0.0f ? 1.0f - snareStrength_ : 1.0f;
	const float desiredDistance = fearTimer_ > 0.0f ? 7.0f : (archetype_ == EnemyArchetype::TorchBearer ? 1.28f : 4.6f);
	const bool lineOfSight = HasLineOfSight({position_.x, position_.y + 1.15f, position_.z},
		{playerPosition.x, playerPosition.y + 1.05f, playerPosition.z}, obstacles);
	const bool shouldMove = fearTimer_ > 0.0f ? distance < desiredDistance :
		(distance > desiredDistance || (archetype_ == EnemyArchetype::WitchHunter && !lineOfSight));
	if (shouldMove) {
		moveAndCollide(Vector3Scale(moveDirection, moveSpeed_ * snareMultiplier * dt), obstacles);
	}

	const float attackRange = archetype_ == EnemyArchetype::TorchBearer ? 1.55f : 5.2f;
	if (fearTimer_ <= 0.0f && lineOfSight && distance <= attackRange && attackCooldown_ <= 0.0f) {
		attackCooldown_ = archetype_ == EnemyArchetype::TorchBearer ? 0.92f : 1.35f;
		attackFlash_ = 1.0f;
		return attackDamage_;
	}
	return 0.0f;
}

float Enemy::applyHex(const SpellStats& stats, float damageScale) {
	if (!isAlive()) {
		return 0.0f;
	}
	const float damage = Actor::takeDamage(stats.damage * damageScale);
	if (damage > 0.0f) {
		hitFlash_ = 1.0f;
	}
	if (isAlive() && stats.poisonDps > 0.0f) {
		poisonDps_ = std::max(poisonDps_, stats.poisonDps);
		poisonTimer_ = std::max(poisonTimer_, stats.duration);
	}
	if (isAlive() && stats.snareStrength > 0.0f) {
		snareStrength_ = std::max(snareStrength_, stats.snareStrength);
		snareTimer_ = std::max(snareTimer_, stats.duration);
	}
	if (isAlive() && stats.fearDuration > 0.0f) {
		fearTimer_ = std::max(fearTimer_, stats.fearDuration);
	}
	return damage;
}

void Enemy::drawBillboardHud(const Camera3D& camera) const {
	if (!isAlive()) {
		return;
	}
	const Vector3 toEnemy = Vector3Subtract(position_, camera.position);
	const Vector3 view = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
	if (Vector3DotProduct(toEnemy, view) <= 0.0f) {
		return;
	}
	const Vector2 screen = GetWorldToScreen({position_.x, position_.y + height_ + 0.22f, position_.z}, camera);
	if (screen.x < -60.0f || screen.x > GetScreenWidth() + 60.0f || screen.y < -30.0f || screen.y > GetScreenHeight() + 30.0f) {
		return;
	}
	const float width = archetype_ == EnemyArchetype::TorchBearer ? 46.0f : 58.0f;
	DrawRectangle(static_cast<int>(screen.x - width / 2.0f), static_cast<int>(screen.y), static_cast<int>(width), 5, {10, 13, 17, 220});
	DrawRectangle(static_cast<int>(screen.x - width / 2.0f + 1.0f), static_cast<int>(screen.y + 1.0f),
		static_cast<int>((width - 2.0f) * health_ / maxHealth_), 3, {255, 79, 88, 255});
}

EnemyArchetype Enemy::archetype() const {
	return archetype_;
}

ReagentType Enemy::dropType() const {
	return dropType_;
}

bool Enemy::claimDrop() {
	if (isAlive() || dropClaimed_) {
		return false;
	}
	dropClaimed_ = true;
	return true;
}

float Enemy::targetRadius() const {
	return collisionRadius_ + 0.18f;
}

void Enemy::onDraw(const Camera3D& camera) const {
	Vector3 toCamera = Vector3Subtract(camera.position, position_);
	toCamera.y = 0.0f;
	if (Vector3LengthSqr(toCamera) < 0.001f) {
		toCamera = {0.0f, 0.0f, 1.0f};
	} else {
		toCamera = Vector3Normalize(toCamera);
	}
	const Vector3 right {toCamera.z, 0.0f, -toCamera.x};
	Color color = archetype_ == EnemyArchetype::TorchBearer ? Color {255, 192, 91, 255} : Color {255, 88, 105, 255};
	if (poisonTimer_ > 0.0f) color = {179, 255, 83, 255};
	if (fearTimer_ > 0.0f) color = {255, 121, 227, 255};
	if (hitFlash_ > 0.0f) color = WHITE;

	const float sway = std::sin(walkPhase_) * 0.05f;
	const Vector3 hip = BillboardPoint(position_, right, sway, 0.78f);
	const Vector3 shoulder = BillboardPoint(position_, right, -sway * 0.5f, 1.42f);
	const Vector3 head {shoulder.x, position_.y + 1.68f, shoulder.z};
	DrawSphereWires(head, archetype_ == EnemyArchetype::TorchBearer ? 0.18f : 0.20f, 5, 8, color);
	DrawLine3D(BillboardPoint(position_, right, -0.33f, 1.50f), BillboardPoint(position_, right, 0.33f, 1.50f), color);
	DrawLine3D(BillboardPoint(position_, right, -0.27f, 1.78f), BillboardPoint(position_, right, 0.30f, 1.78f), color);
	DrawLine3D(shoulder, hip, color);
	DrawLine3D(BillboardPoint(position_, right, -0.31f, 1.45f), BillboardPoint(position_, right, -0.48f, 0.86f), color);
	DrawLine3D(BillboardPoint(position_, right, 0.31f, 1.45f), BillboardPoint(position_, right, 0.48f, 0.86f), color);
	DrawLine3D(hip, BillboardPoint(position_, right, -0.30f, 0.06f), color);
	DrawLine3D(hip, BillboardPoint(position_, right, 0.30f, 0.06f), color);
	DrawLine3D(BillboardPoint(position_, right, -0.33f, 0.06f), BillboardPoint(position_, right, -0.12f, 0.06f), color);
	DrawLine3D(BillboardPoint(position_, right, 0.12f, 0.06f), BillboardPoint(position_, right, 0.33f, 0.06f), color);

	if (archetype_ == EnemyArchetype::TorchBearer) {
		const Vector3 hand = BillboardPoint(position_, right, 0.48f, 0.90f);
		const Vector3 torchTop = BillboardPoint(position_, right, 0.50f, 2.08f);
		DrawLine3D(hand, torchTop, {226, 213, 170, 255});
		DrawSphereWires(torchTop, 0.12f + attackFlash_ * 0.05f, 4, 7, {255, 91, 47, 255});
	} else {
		const Vector3 hand = BillboardPoint(position_, right, -0.46f, 1.05f);
		const Vector3 weapon = BillboardPoint(position_, right, 0.46f, 1.05f);
		DrawLine3D(hand, weapon, {115, 226, 255, 255});
	}
}

void Enemy::moveAndCollide(Vector3 delta, const std::vector<BoundingBox>& obstacles) {
	Vector3 candidate = position_;
	candidate.x += delta.x;
	if (!collidesAt(candidate, obstacles)) {
		position_.x = candidate.x;
	}
	candidate = position_;
	candidate.z += delta.z;
	if (!collidesAt(candidate, obstacles)) {
		position_.z = candidate.z;
	}
}

bool Enemy::collidesAt(Vector3 position, const std::vector<BoundingBox>& obstacles) const {
	for (const BoundingBox obstacle : obstacles) {
		if (CheckCollisionBoxSphere(obstacle, {position.x, position.y + 0.8f, position.z}, collisionRadius_)) {
			return true;
		}
	}
	return false;
}
