#pragma once

#include "Actor.hxx"
#include "CurseVector.hxx"

class Enemy : public Actor {
public:
	Enemy();
	explicit Enemy(Vector3 startPosition);

	void updateAI(float dt, const Vector3& playerPosition);
	void draw3D(const Camera3D& camera) const override;
	void applySpell(const ComposedSpell& spell, const Vector3& sourcePosition, float scale = 1.0f);

	bool canAttack(const Vector3& playerPosition) const;
	float consumeAttack();
	bool hasDroppedLoot() const;
	void markLootDropped();
	float getAttackRange() const;

private:
	float attackCooldown;
	float snareTimer;
	float fearTimer;
	float rotTimer;
	float rotDamagePerSecond;
	bool lootDropped;
};
