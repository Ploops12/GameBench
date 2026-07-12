#pragma once

#include <random>
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
#include "Sprite.hxx"

class Game {
public:
	Game(int screenWidth, int screenHeight);
	~Game();

	void update();
	void draw();
private:
	enum class State {
		MainMenu,
		Playing,
		Victory,
		Defeat
	};

	void loadResources();
	void unloadResources();
	void startRun();
	void buildWorld();
	void spawnWave(int waveIndex);
	void updatePlaying(float dt, const InputHandler::InputState& input);
	void updateEffects(float dt);
	void updatePickups(float dt);
	void updateEnemies(float dt);
	void handleCasting(const InputHandler::InputState& input);
	void cleanDeadEnemies();
	void drawWorld() const;
	Vector3 randomSpawnPoint();

	int screenWidth {1280};
	int screenHeight {720};
	State state {State::MainMenu};
	InputHandler inputHandler {};
	Player player {};
	HUD hud {};
	CoatMenu coatMenu {};
	MainMenu mainMenu {};
	std::vector<Scenery> scenery;
	std::vector<BoundingBox> blockers;
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Effect> effects;
	Sprite handsSprite {};
	Sprite enemySprite {};
	Sprite fenceSprite {};
	Sprite cottageSprite {};
	Sprite wardSprite {};
	float restartTimer {0.0f};
	int wave {0};
	bool laneCleared {false};
	std::mt19937 rng;
};
