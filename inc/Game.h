#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>
#include <raylib.h>
#include "CoatMenu.h"
#include "Effect.hxx"
#include "Enemy.hxx"
#include "HUD.h"
#include "InputHandler.hxx"
#include "Pickup.hxx"
#include "Player.hxx"
#include "Scenery.hxx"

struct Projectile {
	Vector3 position{};
	Vector3 velocity{};
	SpellStats stats{};
	float lifetime{3.0f};
	bool seeking{false};
};

struct Mine {
	Vector3 position{};
	SpellStats stats{};
	float lifetime{12.0f};
};

class Game {
public:
	Game();
	~Game();
	void Update(float dt);
	void Draw() const;
	bool ShouldExit() const;

private:
	void LoadAssets();
	void SpawnWorld();
	void UpdateCombat(float dt);
	void DrawWorld() const;
	void CastCurse();
	void CastPoppet();
	void UseApple(const SpellStats& stats);
	void PlaceSpider(const SpellStats& stats);
	void ResolveEnemyHit(Enemy& enemy, const SpellStats& stats);
	void DamageNearby(const Vector3& point, const SpellStats& stats);
	void CollectPickups();
	void SpawnDrop(Vector3 position, int seed);
	void SetMessage(const std::string& message, float seconds = 2.5f);

	InputHandler inputHandler;
	Player player;
	CoatMenu coatMenu;
	HUD hud;
	CurseVector curse;
	std::array<int, REAGENT_COUNT> inventory{};
	std::vector<Enemy> enemies;
	std::vector<Pickup> pickups;
	std::vector<Scenery> scenery;
	std::vector<Projectile> projectiles;
	std::vector<Mine> mines;
	std::vector<Effect> effects;
	Texture2D enemySheet{};
	bool casting{false};
	float castFlash{0.0f};
	float messageTimer{0.0f};
	std::string message;
	int kills{0};
};
