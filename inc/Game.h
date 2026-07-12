#pragma once

#include <array>
#include <vector>
#include <raylib.h>
#include "CoatMenu.h"
#include "InputHandler.hxx"
#include "Player.hxx"

class Game {
public:
	Game(int screenWidth, int screenHeight);
	void update(float dt);
	void draw() const;
private:
	struct Enemy { Vector3 position{}; float health{48.0f}; float attackTimer{}; float poisonTimer{}; float snareTimer{}; float fearTimer{}; bool dead{}; };
	struct Pickup { Vector3 position{}; ReagentType type{}; bool collected{}; };
	struct Projectile { Vector3 position{}; Vector3 velocity{}; CurseVector spell{}; float life{2.0f}; bool mine{}; };
	int width;
	int height;
	InputHandler input;
	Player player;
	CoatMenu coat;
	std::array<int, static_cast<int>(ReagentType::Count)> inventory{};
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Projectile> projectiles;
	std::vector<BoundingBox> obstacles;
	float castCooldown{};
	float messageTime{};
	char message[96]{"Find reagents. Hold TAB to rummage your coat."};
	void cast();
	void detonate(const CurseVector& spell, Vector3 position);
	void applyApple(const CurseVector& spell);
	void drawWorld() const;
	void drawHud() const;
};
