#pragma once

#include <string>
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
	Game();
	~Game() = default;

	void update();
	void draw();

private:
	void resetRun();
	void buildWorld();
	void castCurrentCurse();
	void collectPickups();
	void collectEnemyDrops();
	void drawWorld() const;
	void showNotification(std::string text, float duration = 2.0f);
	bool isValidTrapPosition(Vector3 position) const;
	int activeTrapCount() const;
	int livingEnemyCount() const;
	std::string assetPath(const char* relativePath) const;

	InputHandler inputHandler;
	MainMenu mainMenu;
	Player player;
	CoatMenu coatMenu;
	HUD hud;
	ReagentInventory inventory;
	std::vector<Scenery> scenery;
	std::vector<BoundingBox> solidBounds;
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Effect> effects;
	Sprite handSprite;
	Sprite enemySprite;
	std::string notification;
	float notificationTime {0.0f};
	bool coatWasOpen {false};
};
