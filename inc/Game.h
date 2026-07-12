#pragma once

#include "CoatMenu.h"
#include "Effect.hxx"
#include "Enemy.hxx"
#include "HUD.h"
#include "InputHandler.hxx"
#include "MainMenu.h"
#include "Pickup.hxx"
#include "Player.hxx"
#include "Scenery.hxx"

#include <string>
#include <vector>

class Game {
public:
	Game(int screenWidth, int screenHeight);

	void update();
	void draw();

private:
	void startGame();
	void resetGame();
	void createWorld();
	void updatePickups(float deltaTime);
	void updateEnemies(float deltaTime);
	void updateEffects(float deltaTime);
	void castSelectedSpell();
	void applySpellToEnemies(const Effect& effect);
	Enemy* findSeekingTarget(Vector3 origin, float range);
	void announce(const std::string& text, float duration = 1.8f);
	std::vector<BoundingBox> getWorldColliders() const;

	int screenWidth {1280};
	int screenHeight {720};
	InputHandler inputHandler {};
	Player player {};
	HUD hud {};
	CoatMenu coatMenu {};
	MainMenu mainMenu {};
	std::vector<Scenery> scenery {};
	std::vector<Enemy> enemies {};
	std::vector<Pickup> pickups {};
	std::vector<Effect> effects {};
	float elapsedTime {0.0f};
	float announcementTime {0.0f};
	std::string announcement {};
	bool gameStarted {false};
	bool coatWasOpen {false};
};
