#pragma once

#include <vector>
#include "CoatMenu.h"
#include "Enemy.hxx"
#include "Effect.hxx"
#include "HUD.h"
#include "InputHandler.hxx"
#include "Pickup.hxx"
#include "Player.hxx"
#include "Scenery.hxx"

struct Projectile {
	Vector3 position {};
	Vector3 velocity {};
	SpellStats stats {};
	VectorType type{VectorType::Poppet};
	float life{3.0f};
};

class Game {
public:
	Game();
	void update();
	void draw() const;

private:
	void reset();
	void castSpell();
	void explode(Vector3 position, const SpellStats& stats, bool hazard = false);
	void updateCombat(float dt);
	void resolveWorldCollision(Vector3 previousPosition);
	void drawWorld() const;
	void drawHands() const;
	void say(const char* text);

	InputHandler input;
	Player player;
	CoatMenu coat;
	HUD hud;
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Projectile> projectiles;
	std::vector<Effect> hazards;
	std::vector<Scenery> scenery;
	float castCooldown{0.0f};
	float waveTimer{0.0f};
	float screenFlash{0.0f};
	float messageTime{0.0f};
	const char* message{"Find reagents. Build a hex. Survive Salem."};
	int wave{1};
};
