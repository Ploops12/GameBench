#pragma once

#include <vector>
#include "CoatMenu.h"
#include "Effect.hxx"
#include "Enemy.hxx"
#include "HUD.h"
#include "MainMenu.h"
#include "Player.hxx"
#include "Scenery.hxx"

class Game {
public:
	Game();
	void update(float dt);
	void draw() const;
	bool isWon() const { return won; }

private:
	struct WorldPickup {
		Vector3 position{};
		ReagentType type{ReagentType::GraveSalt};
		bool active{true};
	};
	void castSpell();
	void spawnEncounter();
	void drawWorld() const;
	InputHandler input;
	Player player;
	CoatMenu coatMenu;
	HUD hud;
	MainMenu mainMenu;
	std::vector<Enemy> enemies;
	std::vector<Scenery> scenery;
	std::vector<CurseEffect> effects;
	std::vector<WorldPickup> pickups;
	float messageTimer{0.0f};
	std::string message{"WASD move | Mouse look | LMB cast | R ward | Hold TAB rummage"};
	bool won{false};
	bool started{false};
};
