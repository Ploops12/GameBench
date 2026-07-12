#pragma once

#include <string>
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

	void update();
	void updateWithInput(float dt, const InputHandler::InputState& input);
	void draw() const;
	bool runLogicSmoke(std::string& report);

	const Player& getPlayer() const;
	const CoatMenu& getCoatMenu() const;
	const std::vector<Enemy>& getEnemies() const;
	const std::vector<Pickup>& getPickups() const;
	float getTimeSeconds() const;
	int getWaveIndex() const;

private:
	void buildLevel();
	void handleCasting(const InputHandler::InputState& input);
	void resolvePlayerCollisions(Vector3 previousPosition);
	void collectPickups();
	void updateEffects(float dt);
	void updateEnemies(float dt);
	void spawnWave();
	void spawnEnemy(Vector3 position);
	void spawnPickup(Vector3 position, ReagentType type, int amount);
	void applySpellBurst(const ComposedSpell& spell, const Vector3& center, float radiusScale);

	InputHandler inputHandler;
	Player player;
	CoatMenu coatMenu;
	HUD hud;
	std::vector<Scenery> scenery;
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Effect> effects;
	float timeSeconds;
	int waveIndex;
};
