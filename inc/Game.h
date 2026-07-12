#pragma once

#include "Audio.hxx"
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
	Game();
	void startNewRun();
	void update(float dt);
	void draw() const;
	[[nodiscard]] bool shouldClose() const;
	[[nodiscard]] static bool runDeterministicChecks(std::string& report);

private:
	enum class State {
		MainMenu,
		Playing,
		GameOver,
		Victory
	};

	void resetRun();
	void buildArena();
	void spawnWave(int wave);
	void spawnInitialPickups();
	void updatePlaying(const InputHandler::InputState& input, float dt);
	void updateEnemies(float dt, bool vulnerable);
	void updateEffects(float dt);
	void updatePickups(float dt);
	void castSpell();
	void detonate(Vector3 position, const SpellStats& stats, bool createLingeringField = true);
	void applyFieldPulse(const HexFieldEffect& field);
	void collectEnemyDrops();
	void drawWorld() const;
	void drawArenaGround() const;
	void drawWorldLabels() const;
	[[nodiscard]] std::vector<BoundingBox> collisionGeometry() const;
	[[nodiscard]] int livingEnemyCount() const;
	[[nodiscard]] bool findHomingTarget(Vector3 position, Vector3& target) const;

	InputHandler input_;
	AudioFeedback audio_;
	Player player_;
	CurseVector spell_;
	CoatMenu coatMenu_;
	HUD hud_;
	MainMenu mainMenu_;
	std::vector<Scenery> scenery_;
	std::vector<Enemy> enemies_;
	std::vector<Pickup> pickups_;
	std::vector<ProjectileEffect> projectiles_;
	std::vector<TrapEffect> traps_;
	std::vector<HexFieldEffect> fields_;
	std::vector<BurstEffect> bursts_;
	std::vector<TraceEffect> traces_;
	State state_ {State::MainMenu};
	int wave_ {1};
	int kills_ {0};
	float nextWaveTimer_ {0.0f};
	bool closeRequested_ {false};
	bool wardWasActive_ {false};
};
