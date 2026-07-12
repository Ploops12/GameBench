#pragma once

#include "Actor.hxx"

class Enemy : public Actor {
public:
	Enemy(Vector3 start, int variant = 0);
	void update(float dt, Vector3 playerPosition, bool playerVulnerable);
	void draw() const override;
	void takeDamage(int damage, float snare = 0.0f);
	void panic(float duration) { fearTimer = duration; }
	void applyRot(int damage, float duration) { rotDamage = damage; rotTimer = duration; rotTick = 0.0f; }
	bool isDead() const { return !active; }
	int getHealth() const { return health; }
	float getAttackTimer() const { return attackTimer; }
	bool canAttack() const;
	void consumeAttack() { attackTimer = 0.8f; }
	bool takeDrop() { if (dropClaimed) return false; dropClaimed = true; return true; }
	int getVariant() const { return variant; }

private:
	int health{38};
	int variant{0};
	float attackTimer{0.0f};
	float snareTimer{0.0f};
	float fearTimer{0.0f};
	int rotDamage{0};
	float rotTimer{0.0f};
	float rotTick{0.0f};
	bool dropClaimed{false};
};
