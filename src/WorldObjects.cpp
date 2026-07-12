#include "Enemy.hxx"
#include "Pickup.hxx"
#include "Player.hxx"
#include "Scenery.hxx"

#include <raymath.h>
#include <algorithm>
#include <cmath>

Enemy::Enemy(Vector3 position, int kind) : Actor(position), kind(kind) {
	health = maxHealth = 42.0f + kind * 16.0f;
	collisionRadius = 0.55f;
}

void Enemy::update(float dt, Player& player) {
	attackTimer -= dt;
	hitFlash = std::max(0.0f, hitFlash - dt);
	snareTimer = std::max(0.0f, snareTimer - dt);
	fearTimer = std::max(0.0f, fearTimer - dt);
	if (poisonTimer > 0.0f) {
		health -= poisonDps * dt;
		poisonTimer -= dt;
	}
	Vector3 toPlayer = Vector3Subtract(player.position, position);
	toPlayer.y = 0.0f;
	const float distance = Vector3Length(toPlayer);
	if (distance > 0.01f) {
		const float direction = fearTimer > 0.0f ? -1.0f : 1.0f;
		const float speed = (2.1f + kind * 0.35f) * (snareTimer > 0.0f ? 0.25f : 1.0f);
		position = Vector3Add(position, Vector3Scale(Vector3Normalize(toPlayer), speed * direction * dt));
	}
	if (distance < 1.35f && attackTimer <= 0.0f && fearTimer <= 0.0f) {
		player.damage(11.0f + kind * 2.0f);
		attackTimer = 0.8f + kind * 0.15f;
	}
}

void Enemy::hurt(float damage, float poison, float snare, float fear) {
	health -= damage;
	poisonDps = std::max(poisonDps, poison);
	poisonTimer = std::max(poisonTimer, poison > 0.0f ? 3.0f : 0.0f);
	snareTimer += snare;
	fearTimer += fear;
	hitFlash = 0.12f;
}

void Enemy::draw() const {
	const Color body = hitFlash > 0.0f ? RAYWHITE : (kind == 0 ? Color{188, 72, 58, 255} : Color{222, 151, 61, 255});
	DrawCylinder(position, 0.42f, 0.58f, 1.15f, 6, body);
	DrawCylinderWires(position, 0.42f, 0.58f, 1.15f, 6, BLACK);
	DrawSphere({position.x, position.y + 0.85f, position.z}, 0.32f, body);
	DrawSphereWires({position.x, position.y + 0.85f, position.z}, 0.32f, 6, 6, BLACK);
	DrawLine3D({position.x - 0.7f, position.y + 1.35f, position.z}, {position.x + 0.7f, position.y + 1.35f, position.z}, BLACK);
	DrawLine3D({position.x - 0.45f, position.y + 1.55f, position.z}, {position.x + 0.45f, position.y + 1.55f, position.z}, body);
}

Pickup::Pickup(Vector3 position, ReagentType type) : Object(position), type(type) {}

void Pickup::update(float dt) {
	age += dt;
}

void Pickup::draw() const {
	Vector3 p = position;
	p.y += 0.45f + std::sin(age * 3.0f) * 0.12f;
	const Color color = GetReagentInfo(type).color;
	DrawSphere(p, 0.18f, color);
	DrawSphereWires(p, 0.3f, 5, 5, color);
	DrawLine3D({p.x, 0.05f, p.z}, p, Fade(color, 0.5f));
}

Scenery::Scenery(BoundingBox bounds) : Object(Vector3Scale(Vector3Add(bounds.min, bounds.max), 0.5f)), collisionBounds(bounds) {
	collidable = true;
}

void Scenery::draw() const {
	const Vector3 size = Vector3Subtract(collisionBounds.max, collisionBounds.min);
	DrawCube(position, size.x, size.y, size.z, Color{24, 29, 27, 255});
	DrawBoundingBox(collisionBounds, Color{229, 205, 137, 255});
	DrawLine3D(collisionBounds.min, collisionBounds.max, Color{235, 86, 68, 255});
}
