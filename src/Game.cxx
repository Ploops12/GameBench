#include "Game.h"

#include <raymath.h>
#include <algorithm>
#include <cmath>

namespace {
	const char* AssetPath(const char* relative) {
		static char path[512];
		if (FileExists(relative)) return relative;
		TextCopy(path, TextFormat("../%s", relative));
		return path;
	}

	int ReagentIndex(ReagentType type) {
		return static_cast<int>(type);
	}
}

Game::Game() {
	LoadAssets();
	SpawnWorld();
	SetMessage("THE CHURCH YARD IS AWAKE // CLEAR THE HUNTERS");
}

Game::~Game() {
	if (enemySheet.id != 0) UnloadTexture(enemySheet);
}

void Game::LoadAssets() {
	enemySheet = LoadTexture(AssetPath("resources/generated/enemies/witch_hunter_sheet.png"));
}

void Game::SpawnWorld() {
	// Low walls make movement and collision readable while leaving the arena open for a boomer-shooter loop.
	scenery.emplace_back(Vector3{-14.0f, 1.4f, -3.0f}, Vector3{0.8f, 2.8f, 30.0f});
	scenery.emplace_back(Vector3{14.0f, 1.4f, -3.0f}, Vector3{0.8f, 2.8f, 30.0f});
	scenery.emplace_back(Vector3{0.0f, 1.4f, -18.0f}, Vector3{28.0f, 2.8f, 0.8f});
	scenery.emplace_back(Vector3{-6.0f, 1.2f, -5.0f}, Vector3{3.0f, 2.4f, 2.0f});
	scenery.emplace_back(Vector3{7.0f, 1.2f, -9.0f}, Vector3{4.0f, 2.4f, 2.0f});
	scenery.emplace_back(Vector3{-8.0f, 1.2f, 3.0f}, Vector3{3.0f, 2.4f, 2.0f});

	enemies.emplace_back(Vector3{-3.6f, 1.05f, -4.0f}, 0);
	enemies.emplace_back(Vector3{0.0f, 1.05f, -7.0f}, 1);
	enemies.emplace_back(Vector3{3.7f, 1.05f, -5.0f}, 0);
	enemies.emplace_back(Vector3{-7.0f, 1.05f, -11.0f}, 2);
	enemies.emplace_back(Vector3{7.3f, 1.05f, -13.0f}, 1);
	enemies.emplace_back(Vector3{0.0f, 1.05f, -15.0f}, 2);

	pickups.emplace_back(Vector3{-2.0f, 0.65f, 4.2f}, ReagentType::GraveSalt, 2);
	pickups.emplace_back(Vector3{2.0f, 0.65f, 2.5f}, ReagentType::WidowNettle, 2);
	pickups.emplace_back(Vector3{-5.0f, 0.65f, -1.5f}, ReagentType::MothDust, 1);
	pickups.emplace_back(Vector3{5.5f, 0.65f, -2.0f}, ReagentType::SaintAsh, 2);
	pickups.emplace_back(Vector3{-10.0f, 0.65f, -8.0f}, ReagentType::BlackWax, 2);
	pickups.emplace_back(Vector3{10.0f, 0.65f, -8.5f}, ReagentType::BellBronze, 2);
}

void Game::Update(float dt) {
	InputHandler::InputState input = inputHandler.poll();
	if (input.coatPressed || input.coatHeld) {
		const bool wasOpen = coatMenu.IsOpen();
		if (input.coatPressed || !coatMenu.IsOpen()) coatMenu.Toggle();
		if (wasOpen != coatMenu.IsOpen()) inputHandler.SetMouseCaptured(!coatMenu.IsOpen());
	}
	if (coatMenu.IsOpen()) {
		if (IsKeyPressed(KEY_ONE)) coatMenu.SelectVector(curse, VectorType::Bottle);
		if (IsKeyPressed(KEY_TWO)) coatMenu.SelectVector(curse, VectorType::Apple);
		if (IsKeyPressed(KEY_THREE)) coatMenu.SelectVector(curse, VectorType::Spider);
		const ReagentType keys[REAGENT_COUNT] = {ReagentType::GraveSalt, ReagentType::SaintAsh, ReagentType::BlackWax,
			ReagentType::WidowNettle, ReagentType::BellBronze, ReagentType::MothDust};
		for (int i = 0; i < REAGENT_COUNT; ++i) {
			const int keyCodes[REAGENT_COUNT] = {KEY_Q, KEY_E, KEY_G, KEY_H, KEY_J, KEY_K};
			if (IsKeyPressed(keyCodes[i]) && coatMenu.AddReagent(curse, keys[i], inventory)) {
				SetMessage(TextFormat("ADDED %s", ReagentName(keys[i])), 1.1f);
			}
		}
		if (IsKeyPressed(KEY_X)) curse.ClearReagents();
		coatMenu.Update(curse, inventory);
	}

	const std::vector<BoundingBox> obstacles = [&]() {
		std::vector<BoundingBox> result;
		for (const Scenery& item : scenery) {
			if (item.IsCollidable()) result.push_back(item.GetBounds());
		}
		return result;
	}();
	player.Update(input, dt, coatMenu.IsOpen(), obstacles);
	if (input.wardPressed) {
		player.ActivateWard();
		if (player.IsWardActive()) {
			effects.emplace_back(EffectType::WardPulse, player.GetPosition(), 1.2f, 0.85f);
			SetMessage("WARD RAISED // THE HUNTERS CANNOT TOUCH YOU", 1.4f);
		}
	}
	if (input.castPressed && !coatMenu.IsOpen() && player.IsAlive()) CastCurse();

	UpdateCombat(dt);
	CollectPickups();
	castFlash = std::fmax(0.0f, castFlash - dt);
	casting = castFlash > 0.0f;
	messageTimer = std::fmax(0.0f, messageTimer - dt);
	if (!player.IsAlive() && IsKeyPressed(KEY_ENTER)) {
		player = Player();
		SetMessage("THE HEX BREAKS // TRY AGAIN");
	}
}

void Game::UpdateCombat(float dt) {
	for (Pickup& pickup : pickups) pickup.Update(dt);
	for (Projectile& projectile : projectiles) {
		if (projectile.seeking) {
			Enemy* closest = nullptr;
			float closestDistance = 8.0f;
			for (Enemy& enemy : enemies) {
				if (enemy.IsDead()) continue;
				const float distance = Vector3Distance(projectile.position, enemy.GetPosition());
				if (distance < closestDistance) {
					closestDistance = distance;
					closest = &enemy;
				}
			}
			if (closest != nullptr) {
				const Vector3 desired = Vector3Normalize(Vector3Subtract(closest->GetPosition(), projectile.position));
				projectile.velocity = Vector3Normalize(Vector3Add(Vector3Scale(projectile.velocity, 0.88f), Vector3Scale(desired, 7.0f)));
				projectile.velocity = Vector3Scale(projectile.velocity, 18.0f);
			}
		}
		projectile.position = Vector3Add(projectile.position, Vector3Scale(projectile.velocity, dt));
		projectile.lifetime -= dt;
	}
	for (int i = static_cast<int>(projectiles.size()) - 1; i >= 0; --i) {
		bool hit = projectiles[i].lifetime <= 0.0f;
		if (!hit) {
			for (Enemy& enemy : enemies) {
				if (!enemy.IsDead() && Vector3Distance(projectiles[i].position, enemy.GetPosition()) < 0.85f) {
					ResolveEnemyHit(enemy, projectiles[i].stats);
					effects.emplace_back(EffectType::Impact, projectiles[i].position, 0.65f + projectiles[i].stats.splashRadius, 0.35f);
					hit = true;
					break;
				}
			}
		}
		if (hit) projectiles.erase(projectiles.begin() + i);
	}

	for (Mine& mine : mines) mine.lifetime -= dt;
	for (int i = static_cast<int>(mines.size()) - 1; i >= 0; --i) {
		bool triggered = mines[i].lifetime <= 0.0f;
		for (Enemy& enemy : enemies) {
			if (!enemy.IsDead() && Vector3Distance(mines[i].position, enemy.GetPosition()) < 1.25f) {
				triggered = true;
				break;
			}
		}
		if (triggered) {
			DamageNearby(mines[i].position, mines[i].stats);
			effects.emplace_back(EffectType::Hazard, mines[i].position, 1.0f + mines[i].stats.splashRadius, 0.8f);
			mines.erase(mines.begin() + i);
		}
	}

	for (Effect& effect : effects) effect.Update(dt);
	effects.erase(std::remove_if(effects.begin(), effects.end(), [](const Effect& effect) { return !effect.IsAlive(); }), effects.end());

	for (Enemy& enemy : enemies) {
		if (enemy.IsDead()) continue;
		enemy.Update(dt, player.GetPosition(), !coatMenu.IsOpen());
		if (Vector3Distance(enemy.GetPosition(), player.GetPosition()) < 1.8f && enemy.CanAttack()) {
			const float damage = player.TakeDamage(enemy.GetAttackDamage());
			enemy.ResetAttack();
			if (damage > 0.0f) SetMessage(TextFormat("A HUNTER STRIKES // -%.0f HEALTH", damage), 0.9f);
		}
	}
	for (int i = static_cast<int>(enemies.size()) - 1; i >= 0; --i) {
		if (enemies[i].IsDead()) {
			SpawnDrop(enemies[i].GetPosition(), kills);
			++kills;
			effects.emplace_back(EffectType::Impact, enemies[i].GetPosition(), 1.0f, 0.65f);
			enemies.erase(enemies.begin() + i);
			SetMessage(TextFormat("HUNTER BROKEN // REAGENT MARKED %d", kills), 1.3f);
		}
	}
}

void Game::CollectPickups() {
	for (int i = static_cast<int>(pickups.size()) - 1; i >= 0; --i) {
		if (Vector3Distance(player.GetPosition(), pickups[i].GetPosition()) < 1.15f) {
			const int index = ReagentIndex(pickups[i].GetType());
			inventory[index] += pickups[i].GetAmount();
			SetMessage(TextFormat("COLLECTED %d %s", pickups[i].GetAmount(), ReagentName(pickups[i].GetType())), 1.2f);
			pickups.erase(pickups.begin() + i);
		}
	}
}

void Game::CastCurse() {
	castFlash = 0.18f;
	if (curse.GetReagentCount() == 0) {
		int total = 0;
		for (int amount : inventory) total += amount;
		if (total == 0) {
			CastPoppet();
			return;
		}
	}
	const SpellStats stats = curse.CalculateStats();
	curse.ConsumeReagents(inventory);
	switch (curse.GetVector()) {
		case VectorType::Bottle: {
			Projectile projectile;
			projectile.position = Vector3Add(player.GetPosition(), Vector3Scale(player.GetForward(), 0.75f));
			projectile.velocity = Vector3Scale(player.GetForward(), 18.0f);
			projectile.stats = stats;
			projectile.seeking = stats.seeks;
			projectiles.push_back(projectile);
			SetMessage("BOTTLE HEX THROWN // THE CURSE HUNTS", 1.2f);
			break;
		}
		case VectorType::Apple: UseApple(stats); break;
		case VectorType::Spider: PlaceSpider(stats); break;
	}
	curse.ClearReagents();
}

void Game::CastPoppet() {
	Projectile projectile;
	projectile.position = Vector3Add(player.GetPosition(), Vector3Scale(player.GetForward(), 0.75f));
	projectile.velocity = Vector3Scale(player.GetForward(), 20.0f);
	projectile.stats.damage = 6.0f;
	projectiles.push_back(projectile);
	SetMessage("POPPET WRENCH // WEAK HEX READY", 1.2f);
}

void Game::UseApple(const SpellStats& stats) {
	player.Heal(18.0f + stats.damage * 0.25f);
	player.GainWard(stats.wardGain);
	effects.emplace_back(EffectType::WardPulse, player.GetPosition(), 0.8f + stats.wardGain * 0.01f, 0.65f);
	SetMessage(TextFormat("APPLE OMEN EATEN // HEALTH +%.0f // WARD +%.0f", 18.0f + stats.damage * 0.25f, stats.wardGain), 1.5f);
}

void Game::PlaceSpider(const SpellStats& stats) {
	Mine mine;
	mine.position = Vector3Add(player.GetPosition(), Vector3Scale(player.GetForward(), 1.25f));
	mine.position.y = 0.18f;
	mine.stats = stats;
	mines.push_back(mine);
	SetMessage("SPIDER NEST PLANTED // LURE THE HUNTERS", 1.3f);
}

void Game::ResolveEnemyHit(Enemy& enemy, const SpellStats& stats) {
	const bool killed = enemy.TakeDamage(stats.damage);
	enemy.AddStatus(stats.snareSeconds, stats.fearSeconds, stats.poisonDamage, stats.hazardSeconds);
	if (stats.splashRadius > 0.0f) DamageNearby(enemy.GetPosition(), stats);
	if (killed) SetMessage("CURSE LANDED // BODY WILL YIELD A REAGENT", 1.0f);
}

void Game::DamageNearby(const Vector3& point, const SpellStats& stats) {
	if (stats.splashRadius <= 0.0f) return;
	for (Enemy& enemy : enemies) {
		if (!enemy.IsDead() && Vector3Distance(point, enemy.GetPosition()) <= stats.splashRadius) {
			enemy.TakeDamage(stats.damage * 0.55f);
			enemy.AddStatus(stats.snareSeconds, stats.fearSeconds, stats.poisonDamage, stats.hazardSeconds);
		}
	}
}

void Game::SpawnDrop(Vector3 dropPosition, int seed) {
	dropPosition.y = 0.65f;
	const ReagentType type = static_cast<ReagentType>(seed % REAGENT_COUNT);
	pickups.emplace_back(dropPosition, type, 1 + (seed % 2));
}

void Game::SetMessage(const std::string& value, float seconds) {
	message = value;
	messageTimer = seconds;
}

void Game::DrawWorld() const {
	DrawPlane({0.0f, 0.0f, -3.0f}, {28.0f, 30.0f}, Color{13, 19, 25, 255});
	DrawGrid(30, 1.0f);
	for (const Scenery& item : scenery) item.Draw();
	for (const Pickup& pickup : pickups) pickup.DrawWorld();
	for (const Enemy& enemy : enemies) enemy.Draw(player.GetCamera(), enemySheet);
	for (const Projectile& projectile : projectiles) {
		DrawSphere(projectile.position, 0.18f, projectile.seeking ? SKYBLUE : ORANGE);
		DrawSphereWires(projectile.position, 0.25f, 8, 8, WHITE);
	}
	for (const Mine& mine : mines) {
		DrawCylinder(mine.position, 0.45f, 0.25f, 0.16f, 10, Fade(PURPLE, 0.75f));
		DrawCylinderWires(mine.position, 0.45f, 0.25f, 0.16f, 10, MAGENTA);
	}
	for (const Effect& effect : effects) effect.Draw();
}

void Game::Draw() const {
	BeginDrawing();
	ClearBackground(Color{8, 12, 19, 255});
	BeginMode3D(player.GetCamera());
	DrawWorld();
	EndMode3D();
	hud.Draw(player, curse, inventory, coatMenu.IsOpen(), kills, message, messageTimer);
	if (coatMenu.IsOpen()) coatMenu.Draw(inventory, curse);
	hud.DrawHands(player.IsWardActive(), casting);
	DrawText(TextFormat("HUNTERS REMAINING %d", static_cast<int>(enemies.size())), GetScreenWidth() / 2 - 90, 22, 16, ORANGE);
	if (!player.IsAlive()) {
		DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.62f));
		DrawText("THE HUNTERS HAVE YOUR NAME", GetScreenWidth() / 2 - 178, GetScreenHeight() / 2 - 22, 24, RED);
		DrawText("ENTER TO RETURN TO THE CHURCH YARD", GetScreenWidth() / 2 - 174, GetScreenHeight() / 2 + 18, 16, BEIGE);
	}
	EndDrawing();
}

bool Game::ShouldExit() const {
	return WindowShouldClose();
}
