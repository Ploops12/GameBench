#pragma once

#include <string>
#include <vector>
#include "CoatMenu.h"
#include "Effect.hxx"
#include "Enemy.hxx"
#include "HUD.h"
#include "InputHandler.hxx"
#include "MainMenu.h"
#include "Pickup.hxx"
#include "Player.hxx"
#include "Scenery.hxx"

class Game {
public:
	Game();

	void update(float dt);
	void draw() const;
	void reset();

private:
	void castSpell();
	void applySpellArea(Vector3 center, const SpellStats& stats, float radiusScale = 1.0f);
	void updateEffects(float dt);
	void updateEnemies(float dt);
	void collectPickups();
	void setMessage(const std::string& value, float duration = 2.0f);
	std::vector<BoundingBox> obstacleBounds() const;

	InputHandler input;
	MainMenu mainMenu;
	Player player;
	CoatMenu coat;
	HUD hud;
	std::vector<Scenery> scenery;
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Effect> effects;
	std::string message;
	float messageTimer {};
	float elapsed {};
	int collected {};
	bool started {};
};
