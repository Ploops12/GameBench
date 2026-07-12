#pragma once

#include <vector>
#include "Actor.hxx"
#include "CurseVector.hxx"

class Enemy : public Actor {
public:
	Enemy(Vector3 position = {}, int kind = 0);
	using Actor::update;

	void update(float dt, Vector3 playerPosition, const std::vector<BoundingBox>& obstacles);
	void draw3D() const override;
	void applyHex(const SpellStats& stats);
	bool canAttack() const { return attackCooldown <= 0.0f; }
	void resetAttack() { attackCooldown = 0.85f; }
	int getKind() const { return kind; }
	float getSnareTimer() const { return snareTimer; }
	float getFearTimer() const { return fearTimer; }
	BoundingBox getBoundsAt(Vector3 value) const;

private:
	int kind {};
	float attackCooldown {};
	float snareTimer {};
	float fearTimer {};
	float rotTimer {};
	float rotDamagePerSecond {};
	float flashTimer {};
};
