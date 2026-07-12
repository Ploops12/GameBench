#pragma once

#include "Actor.hxx"
#include "CurseVector.hxx"

enum class EnemyType {
	TorchBearer,
	WitchHunter,
	Hound
};

class Enemy : public Actor {
public:
	Enemy(EnemyType type, Vector3 position);

	void update(float deltaTime, Vector3 playerPosition);
	void draw3D() const override;
	void applySpell(const SpellStats& spell, float damageScale = 1.0f);
	float consumePendingDamage();
	bool justDied();
	bool hasDiedHandled() const;
	void markDeathHandled();
	EnemyType getType() const;
	const char* getName() const;
	float getFearRemaining() const;
	float getSnareRemaining() const;

private:
	EnemyType type {EnemyType::TorchBearer};
	float movementSpeed {2.5f};
	float attackRange {1.3f};
	float attackDamage {8.0f};
	float attackTimer {0.0f};
	float pendingDamage {0.0f};
	float rotRemaining {0.0f};
	float rotDamagePerSecond {0.0f};
	float rotTimer {0.0f};
	float snareRemaining {0.0f};
	float fearRemaining {0.0f};
	float hitFlash {0.0f};
	bool deathHandled {false};
};
