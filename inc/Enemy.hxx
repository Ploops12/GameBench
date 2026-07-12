#pragma once

#include "Actor.hxx"
#include "Reagent.hxx"

class Player;

class Enemy : public Actor {
public:
	explicit Enemy(Vector3 position = {});

	void update(Player& player, float deltaTime);
	void applyCurse(const struct SpellStats& spell);
	void draw3D() const override;
	bool canDropPickup() const;
	ReagentType getDropType() const;

private:
	float attackTimer {};
	float poisonTime {};
	float poisonDamagePerSecond {};
	float snareTime {};
	float fearTime {};
	ReagentType dropType {ReagentType::GraveSalt};
};
