#pragma once

#include <vector>

#include "Audio.hxx"
#include "Assets.hxx"
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
	// Owns the arena and advances every combat system, including while the coat is open.
public:
	Game();
	~Game();
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(Game&&) = delete;

	void update();
	void draw() const;

private:
	enum class State {
		Title,
		Playing,
		Defeat
	};

	struct PlayerProjectile {
		Vector3 position {};
		Vector3 previousPosition {};
		Vector3 velocity {};
		SpellStats stats {};
		float radius {0.16f};
		float life {5.0f};
		bool active {true};
	};

	struct HostileProjectile {
		Vector3 position {};
		Vector3 velocity {};
		float damage {0.0f};
		float life {5.0f};
		bool active {true};
	};

	struct SpiderMine {
		Vector3 position {};
		SpellStats stats {};
		float armTime {0.55f};
		float life {18.0f};
		bool active {true};
	};

	void resetRun();
	void buildArena();
	void spawnStartingPickups();
	void spawnWave();
	bool castPreparedSpell();
	void castPoppet();
	void resolveSpell(Vector3 position, const SpellStats& stats, Enemy* directTarget = nullptr);
	void damagePlayer(float damage);
	void handleMainVectorSelection(const InputHandler::InputState& input);
	void updateProjectiles(float deltaTime);
	void updateMines(float deltaTime);
	void updateEffects(float deltaTime);
	void updateEnemies(float deltaTime);
	void updatePickups(float deltaTime);
	void handleEnemyDeaths();
	void drawWorld() const;
	void drawProjectiles() const;
	bool findSpiderPlacement(Vector3& placement) const;
	std::vector<BoundingBox> getSolidBounds() const;
	std::vector<BoundingBox> getPlayerSolidBounds() const;
	Enemy* findNearestEnemy(Vector3 position, float maxDistance);

	State state {State::Title};
	InputHandler inputHandler;
	AudioFeedback audio;
	GameAssets assets;
	MainMenu mainMenu;
	Player player;
	CoatMenu coatMenu;
	HUD hud;
	std::vector<Scenery> scenery;
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Effect> effects;
	std::vector<PlayerProjectile> playerProjectiles;
	std::vector<HostileProjectile> hostileProjectiles;
	std::vector<SpiderMine> mines;
	int wave {0};
	float nextWaveTime {0.0f};
	float elapsedTime {0.0f};
	bool wardWasActive {false};
};
