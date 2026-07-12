#pragma once

#include "Actor.hxx"
#include "CurseVector.hxx"
#include "Reagent.hxx"

#include <vector>

enum class EnemyArchetype {
	TorchBearer,
	WitchHunter
};

class Enemy : public Actor {
public:
	Enemy(Vector3 position, EnemyArchetype archetype, int threatLevel, ReagentType dropType);

	float updateCombat(float dt, Vector3 playerPosition, const std::vector<BoundingBox>& obstacles);
	float applyHex(const SpellStats& stats, float damageScale = 1.0f);
	void drawBillboardHud(const Camera3D& camera) const;

	[[nodiscard]] EnemyArchetype archetype() const;
	[[nodiscard]] ReagentType dropType() const;
	[[nodiscard]] bool claimDrop();
	[[nodiscard]] float targetRadius() const;

protected:
	void onDraw(const Camera3D& camera) const override;

private:
	void moveAndCollide(Vector3 delta, const std::vector<BoundingBox>& obstacles);
	[[nodiscard]] bool collidesAt(Vector3 position, const std::vector<BoundingBox>& obstacles) const;

	EnemyArchetype archetype_ {EnemyArchetype::TorchBearer};
	ReagentType dropType_ {ReagentType::BoneSplinters};
	float moveSpeed_ {2.3f};
	float attackDamage_ {9.0f};
	float attackCooldown_ {0.5f};
	float poisonDps_ {0.0f};
	float poisonTimer_ {0.0f};
	float snareStrength_ {0.0f};
	float snareTimer_ {0.0f};
	float fearTimer_ {0.0f};
	float hitFlash_ {0.0f};
	float attackFlash_ {0.0f};
	float walkPhase_ {0.0f};
	bool dropClaimed_ {false};
};
