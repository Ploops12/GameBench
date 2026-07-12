#pragma once

#include <vector>

#include "Actor.hxx"
#include "CurseVector.hxx"

class Player;

enum class EnemyArchetype {
	TorchBearer,
	WitchHunter
};

class Enemy : public Actor {
public:
	using Actor::update;

	Enemy(Vector3 position, EnemyArchetype archetype, ReagentType dropType);

	void update(float deltaTime, Player& player, const std::vector<BoundingBox>& solidBounds);
	void draw(const Camera3D& camera) const override;
	void draw(const Camera3D& camera, const Texture2D* billboardTexture) const;
	void applyHex(const SpellStats& spell, float strength = 1.0f);
	void applyThornRetaliation(float damage, float snareDuration);

	bool hasDrop() const;
	bool takeDrop(ReagentType& result);
	EnemyArchetype getArchetype() const;
	Vector3 getTargetPosition() const;
	float getFearTimer() const;
	float getSnareTimer() const;

private:
	bool collidesWithWorld(Vector3 candidate, const std::vector<BoundingBox>& solidBounds) const;
	void markDeathIfNeeded(bool wasAlive);

	EnemyArchetype archetype {EnemyArchetype::TorchBearer};
	ReagentType dropType {ReagentType::GraveSalt};
	float speed {2.1f};
	float attackCooldown {0.0f};
	float snareTimer {0.0f};
	float fearTimer {0.0f};
	float poisonTimer {0.0f};
	float poisonDamagePerSecond {0.0f};
	float hurtTimer {0.0f};
	bool dropAvailable {false};
};
