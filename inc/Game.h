#pragma once

#include "CoatMenu.h"
#include "Effect.hxx"
#include "Enemy.hxx"
#include "HUD.h"
#include "InputHandler.hxx"
#include "Pickup.hxx"
#include "Player.hxx"
#include <raylib.h>
#include <vector>

enum class GameFlowState {
	Playing,
	Victory,
	Defeat
};

struct Obstacle {
	Vector3 center{};
	Vector3 size{};
};

class Game {
public:
	Game(int screenWidth, int screenHeight);

	void update(float dt);
	void draw();

private:
	int screenWidth{};
	int screenHeight{};
	InputHandler inputHandler;
	Player player;
	CoatMenu coatMenu;
	HUD hud;
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Effect> effects;
	std::vector<Obstacle> obstacles;
	GameFlowState flowState{GameFlowState::Playing};
	int wave{0};
	float waveMessageTimer{0.0f};
	float yaw{0.0f};
	float pitch{0.0f};

	void reset();
	void spawnArena();
	void spawnWave(int waveIndex);
	void advanceWaveIfCleared();
	void updatePlayer(float dt, const InputHandler::InputState& input);
	void updateEnemies(float dt);
	void updateEffects(float dt);
	void updatePickups(float dt);
	void castCurrentSpell();
	void triggerSpellImpact(const SpellDraft& draft, const SpellStats& stats, Vector3 position);
	void applySpellToEnemy(Enemy& enemy, const SpellStats& stats, float distance);
	Vector3 resolveWorldCollision(Vector3 position) const;
	bool hasLineOfSight(Vector3 from, Vector3 to) const;
	void drawWorld();
	void drawHands() const;
	void drawFlowOverlay() const;
};
