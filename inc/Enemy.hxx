#pragma once

#include "Actor.hxx"

class Player;

class Enemy : public Actor {
public:
	using Actor::update;
	Enemy(Vector3 position = {}, int kind = 0);
	void update(float dt, Player& player);
	void draw() const override;
	void hurt(float damage, float poison, float snare, float fear);
	bool dead() const { return health <= 0.0f; }

	int kind{0};
	float attackTimer{0.0f};
	float poisonTimer{0.0f};
	float poisonDps{0.0f};
	float snareTimer{0.0f};
	float fearTimer{0.0f};
	float hitFlash{0.0f};
	bool dropped{false};
};
