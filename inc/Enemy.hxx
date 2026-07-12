#pragma once

#include <vector>

#include "Actor.hxx"
#include "Reagent.hxx"

enum class EnemyKind {
	TorchBearer,
	WitchHunter
};

struct EnemyAttackEvent {
	bool valid {false};
	bool projectile {false};
	Vector3 origin {};
	Vector3 direction {};
	float damage {0.0f};
};

class Enemy : public Actor {
	// Tracks the player, responds to curse statuses, and emits readable attacks.
public:
	Enemy(EnemyKind kind, Vector3 position, ReagentType dropType, const Texture2D* spriteSheet = nullptr);

	EnemyAttackEvent updateAI(float deltaTime, Vector3 playerPosition,
		const std::vector<BoundingBox>& solidBounds);
	void draw3D(const Camera3D& camera) const override;
	EnemyKind getKind() const;
	ReagentType getDropType() const;
	bool claimDrop();
	float getCollisionRadius() const;

private:
	bool collidesAt(Vector3 candidate, const std::vector<BoundingBox>& solidBounds) const;
	EnemyKind kind;
	ReagentType dropType;
	const Texture2D* spriteSheet {nullptr};
	float moveSpeed {2.5f};
	float attackCooldown {0.5f};
	float telegraphTime {0.0f};
	float animationTime {0.0f};
	bool attackPending {false};
	bool dropClaimed {false};
};
