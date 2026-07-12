#pragma once

// Billboard enemy actor that tracks, attacks, receives statuses, and drops reagents.

#include "Actor.hxx"
#include "Reagent.hxx"

#include <string>

class Enemy : public Actor {
public:
	Enemy() = default;
	Enemy(Vector3 startPosition, std::string label, float enemyHealth, float enemySpeed, float enemyDamage);

	void updateToward(Vector3 playerPosition, float dt);
	void draw(const Camera3D& camera) const override;
	bool canAttack(Vector3 playerPosition) const;
	float consumeAttackDamage();
	void applyStatus(float snare, float fear, float rotDps, float rotSeconds);
	void tickStatus(float dt);
	bool shouldDrop() const;
	ReagentType dropType() const;
	void setDropType(ReagentType type);

	std::string name{"Torch Mob"};
	float speed{2.5f};
	float damage{9.0f};
	float attackRange{1.35f};
	float attackCooldown{};
	float attackPeriod{0.9f};
	float snareSeconds{};
	float fearSeconds{};
	float rotDps{};
	float rotSeconds{};
	bool dropPending{true};
	ReagentType heldDrop{ReagentType::GraveSalt};
};
