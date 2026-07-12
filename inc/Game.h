#pragma once

#include <vector>

#include "CoatMenu.h"
#include "Effect.hxx"
#include "Enemy.hxx"
#include "HUD.h"
#include "InputHandler.hxx"
#include "Pickup.hxx"
#include "Player.hxx"
#include "Scenery.hxx"

class Game {
public:
	Game();

	void update(float deltaTime);
	void draw() const;

private:
	struct Projectile {
		Vector3 position {};
		Vector3 velocity {};
		SpellStats spell {};
		float timeLeft {};
	};
	struct Mine {
		Vector3 position {};
		SpellStats spell {};
		float armTime {0.35f};
		float timeLeft {12.0f};
	};

	void buildArena();
	void cast();
	void detonate(Vector3 position, const SpellStats& spell);
	void updateProjectiles(float deltaTime);
	void updateMines(float deltaTime);
	void updateEffects(float deltaTime);
	void updatePickups(float deltaTime);
	void drawWorld() const;
	void drawProjectile(const Projectile& projectile) const;
	void drawMine(const Mine& mine) const;

	InputHandler input;
	Player player;
	CoatMenu coat;
	HUD hud;
	std::vector<Scenery> scenery;
	std::vector<BoundingBox> obstacles;
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Projectile> projectiles;
	std::vector<Mine> mines;
	std::vector<Effect> effects;
	float time {};
	float castCooldown {};
	float castFlash {};
};
