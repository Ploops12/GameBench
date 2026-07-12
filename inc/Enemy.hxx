#pragma once

#include <raylib.h>
#include "Actor.hxx"

class Player;

class Enemy : public Actor {
public:
	using Object::Draw;
	using Object::Update;

	Enemy(Vector3 start, int variant = 0);
	void Update(float dt, const Vector3& target, bool targetVulnerable);
	void Draw(const Camera3D& camera, Texture2D spriteSheet) const;
	bool TakeDamage(float amount);
	bool IsDead() const { return health <= 0.0f; }
	int GetVariant() const { return variant; }
	float GetHealth() const { return health; }
	float GetMaxHealth() const { return maxHealth; }
	float GetSnareSeconds() const { return snareSeconds; }
	float GetFearSeconds() const { return fearSeconds; }
	void AddStatus(float snare, float fear, float poison, float duration);
	bool CanAttack() const { return attackTimer <= 0.0f; }
	void ResetAttack() { attackTimer = 1.15f; }
	float GetAttackDamage() const { return 7.0f + variant * 2.0f; }

private:
	float health{42.0f};
	float maxHealth{42.0f};
	float attackTimer{0.0f};
	float snareSeconds{0.0f};
	float fearSeconds{0.0f};
	float poisonDamage{0.0f};
	float poisonTimer{0.0f};
	int variant{0};
};
