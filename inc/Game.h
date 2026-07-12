#pragma once

#include "CoatMenu.h"
#include "Effect.hxx"
#include "Enemy.hxx"
#include "HUD.h"
#include "InputHandler.hxx"
#include "Pickup.hxx"
#include "Player.hxx"
#include "Scenery.hxx"

#include <string>
#include <vector>

class Game {
public:
	Game(int screenWidth, int screenHeight);

	void update(float deltaTime);
	void draw() const;

private:
	void spawnLevel();
	void updateCombat(float deltaTime);
	void updateEffects(float deltaTime);
	void castSpell();
	void restartLevel();
	void applyImpact(Vector3 impactPosition, const CurseVector& spell);
	void collectPickups();
	void setStatus(const std::string& message, float duration = 2.5f);
	std::vector<BoundingBox> collisionBoxes() const;

	void drawWorld() const;
	void drawWorldMarkers() const;

	int width{1280};
	int height{720};
	InputHandler input;
	Player player;
	CoatMenu coatMenu;
	HUD hud;
	std::vector<Scenery> scenery;
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Effect> effects;
	std::string statusMessage{"The road is yours. Gather the six signs."};
	float statusTimer{5.0f};
	float castTimer{0.0f};
	float worldTime{0.0f};
	bool rummageToggle{false};
	bool levelCompleteMessageShown{false};
};
