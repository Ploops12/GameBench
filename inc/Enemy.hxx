#pragma once

#include "Actor.hxx"
#include "CurseVector.hxx"

#include <vector>

enum class EnemyKind {
	TorchBearer,
	WitchHunter,
	Suppressor
};

class Enemy : public Actor {
public:
	Enemy(EnemyKind kind, Vector3 position);

	bool updateTowardsPlayer(float deltaTime, Vector3 playerPosition, const std::vector<BoundingBox>& obstacles);
	void applySpell(const SpellStats& stats);
	void draw() const override;

	EnemyKind kind() const;
	float attackDamage() const;
	float snareTime() const;
	float fearTime() const;
	float poisonTime() const;
	bool recentlyHit() const;
	Color accentColor() const;

private:
	EnemyKind enemyKind{EnemyKind::TorchBearer};
	float attackTimer{0.0f};
	float snareTimer{0.0f};
	float fearTimer{0.0f};
	float poisonTimer{0.0f};
	float poisonDamage{0.0f};
	float hitFlash{0.0f};
	float bobTime{0.0f};
};
