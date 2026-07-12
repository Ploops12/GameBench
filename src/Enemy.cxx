#include "Enemy.hxx"

Enemy::Enemy(Vector3 start, int kind) : variant(kind) {
	position = start;
	radius = 0.48f;
	health = kind == 1 ? 55 : 38;
}

void Enemy::update(float dt, Vector3 playerPosition, bool) {
	if (!active) return;
	attackTimer -= dt;
	snareTimer -= dt;
	fearTimer -= dt;
	rotTimer -= dt;
	rotTick -= dt;
	if (rotTimer > 0.0f && rotTick <= 0.0f) { takeDamage(rotDamage); rotTick = 0.6f; }
	Vector3 delta = Vector3Subtract(playerPosition, position);
	const float distance = Vector3Length(delta);
	if (distance > 2.0f && distance > 0.01f) position = Vector3Add(position, Vector3Scale(Vector3Normalize(delta), dt * (snareTimer > 0.0f ? 0.45f : (fearTimer > 0.0f ? 0.35f : (variant == 1 ? 1.0f : 1.35f)))));
}

void Enemy::takeDamage(int damage, float snare) {
	if (!active) return;
	health -= damage;
	snareTimer = snare;
	if (health <= 0) active = false;
}

bool Enemy::canAttack() const { return active && attackTimer <= 0.0f; }

void Enemy::draw() const {
	if (!active) return;
	const Color outline = variant == 1 ? Color{255, 120, 70, 255} : Color{255, 235, 145, 255};
	const Color coat = variant == 1 ? Color{95, 35, 35, 255} : Color{35, 45, 65, 255};
	DrawCylinder(Vector3{position.x, position.y + 0.75f, position.z}, 0.38f, 0.55f, 0.95f, 6, coat);
	DrawSphere(Vector3{position.x, position.y + 1.45f, position.z}, 0.28f, outline);
	DrawCylinder(Vector3{position.x, position.y + 1.72f, position.z}, 0.48f, 0.02f, 0.12f, 6, BLACK);
	DrawLine3D(Vector3{position.x - 0.48f, position.y + 0.8f, position.z}, Vector3{position.x + 0.48f, position.y + 0.8f, position.z}, outline);
	DrawLine3D(Vector3{position.x, position.y + 0.55f, position.z - 0.28f}, Vector3{position.x, position.y + 0.05f, position.z - 0.28f}, outline);
}
