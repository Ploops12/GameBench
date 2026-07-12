#pragma once

// Main game coordinator: owns player, enemies, pickups, scenery, spell effects, coat menu, HUD, and combat slice state.

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
	Game();
	void update(float dt);
	void draw() const;

private:
	friend bool runVerticalSliceSelfTest(std::string* errorMessage);

	void seedWorld();
	void resetRun();
	void updateWithInput(const InputHandler::InputState& input, float dt);
	void updatePlayer(const InputHandler::InputState& input, float dt);
	void updateEnemies(float dt);
	void updatePickups(float dt);
	void updateProjectiles(float dt);
	void updateMines(float dt);
	void updateHazards(float dt);
	void updateFlashes(float dt);
	void resolvePlayerEnemyCollisions();
	void castCurrentSpell();
	void castPoppet();
	void resolveImpact(Vector3 impactPosition, const CurseSpell& spell);
	void applySpellAt(Vector3 position, const CurseSpell& spell, bool spawnHazard);
	void dropReagent(Vector3 position, ReagentType type, int quantity);
	void movePlayerAxis(Vector3 delta);
	bool playerBlockedAt(Vector3 position) const;
	bool lineHitsScenery(Vector3 start, Vector3 end) const;
	Enemy* nearestEnemy(Vector3 position, float maxDistance);
	int livingEnemyCount() const;
	void drawWorld() const;
	void drawEffects() const;

	InputHandler input;
	Player player;
	CoatMenu coatMenu;
	HUD hud;
	std::vector<Scenery> scenery;
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Projectile> projectiles;
	std::vector<SpiderMine> mines;
	std::vector<LingeringHazard> hazards;
	std::vector<ImpactFlash> flashes;
	float messageTimer{};
	std::string message{"Collect reagents. Hold TAB to rummage the coat."};
	bool districtCleared{};
	bool gameOver{};
};

bool runVerticalSliceSelfTest(std::string* errorMessage);
