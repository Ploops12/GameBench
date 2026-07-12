#pragma once

#include <vector>
#include "Actor.hxx"
#include "Animation.hxx"
#include "CurseVector.hxx"

class Player;

class Enemy : public Actor {
public:
	explicit Enemy(Vector3 position = {});

	void setSprite(Sprite* sprite);
	void updateAI(float dt, const Player& player, const std::vector<BoundingBox>& blockers, const std::vector<Vector3>& neighbors);
	void draw3D(const Camera3D& camera) const override;

	void applySpell(const SpellPreview& spell, const Vector3& source);
	void applyKnockback(const Vector3& impulse);

	bool tryAttack(float dt, Player& player);
	bool isRecentlyHit() const;
	bool shouldDropLoot() const;
	void markLootDropped();
private:
	void resolveCollisions(const std::vector<BoundingBox>& blockers);

	Animation animation {};
	float flashTime {0.0f};
	float rotDamagePerSecond {0.0f};
	float rotTimeRemaining {0.0f};
	float snareTimeRemaining {0.0f};
	float fearTimeRemaining {0.0f};
	float attackCooldown {0.0f};
	bool lootDropped {false};
};
