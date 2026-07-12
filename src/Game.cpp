#include "Game.h"

#include <algorithm>
#include <cmath>

#include <raymath.h>

namespace {
constexpr Color GROUND {17, 22, 27, 255};
constexpr Color GRID {47, 55, 61, 255};
constexpr Color STONE {96, 115, 123, 255};
constexpr Color CURSE {229, 91, 108, 255};

float flatDistance(Vector3 first, Vector3 second) {
	first.y = 0.0f;
	second.y = 0.0f;
	return Vector3Distance(first, second);
}
}

Game::Game() : input(KEY_NULL) {
	buildArena();
	enemies = {
		Enemy({-5.5f, 0.0f, -2.0f}), Enemy({4.5f, 0.0f, -5.0f}), Enemy({-9.0f, 0.0f, 5.0f}),
		Enemy({8.0f, 0.0f, 7.0f}), Enemy({0.0f, 0.0f, -10.0f}), Enemy({11.0f, 0.0f, -1.0f})
	};
	pickups = {
		Pickup({-2.0f, 0.0f, 4.0f}, ReagentType::GraveSalt), Pickup({2.0f, 0.0f, 4.5f}, ReagentType::WidowNettle),
		Pickup({-4.5f, 0.0f, 7.0f}, ReagentType::PlagueHoney), Pickup({5.5f, 0.0f, 4.0f}, ReagentType::MothDust),
		Pickup({-7.0f, 0.0f, -3.0f}, ReagentType::SaintAsh), Pickup({7.0f, 0.0f, -5.5f}, ReagentType::BlackWax)
	};
}

void Game::update(float deltaTime) {
	time += deltaTime;
	InputHandler::InputState state = input.poll();
	coat.setOpen(state.rummageHeld);
	coat.update(player.getInventory());
	player.update(state, deltaTime, obstacles);
	castCooldown = std::max(0.0f, castCooldown - deltaTime);
	castFlash = std::max(0.0f, castFlash - deltaTime);
	if (state.castPressed && !coat.isOpen() && castCooldown <= 0.0f) cast();

	for (Enemy& enemy : enemies) enemy.update(player, deltaTime);
	updateProjectiles(deltaTime);
	updateMines(deltaTime);
	updateEffects(deltaTime);
	updatePickups(deltaTime);

	for (Enemy& enemy : enemies) {
		if (enemy.canDropPickup()) {
			pickups.emplace_back(enemy.getPosition(), enemy.getDropType());
			enemy.setActive(false);
		}
	}
	if (player.isDead()) {
		player = Player();
		projectiles.clear();
		mines.clear();
		effects.clear();
	}
}

void Game::draw() const {
	BeginDrawing();
	ClearBackground({8, 10, 14, 255});
	BeginMode3D(player.getCamera());
	drawWorld();
	EndMode3D();
	hud.draw(player, coat, static_cast<int>(std::count_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) { return enemy.isActive() && !enemy.isDead(); })));
	hud.drawHands(player, castFlash > 0.0f);
	coat.draw(player.getInventory());
	if (player.isDead()) DrawText("YOU WERE HUNTED - REFORMING...", GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 - 60, 20, CURSE);
	EndDrawing();
}

void Game::buildArena() {
	auto addBlock = [this](Vector3 minimum, Vector3 maximum, Color color) {
		BoundingBox box {minimum, maximum};
		scenery.emplace_back(box, color);
		obstacles.push_back(box);
	};
	addBlock({-19.5f, 0.0f, -19.5f}, {-18.8f, 3.0f, 19.5f}, STONE);
	addBlock({18.8f, 0.0f, -19.5f}, {19.5f, 3.0f, 19.5f}, STONE);
	addBlock({-19.5f, 0.0f, -19.5f}, {19.5f, 3.0f, -18.8f}, STONE);
	addBlock({-19.5f, 0.0f, 18.8f}, {19.5f, 3.0f, 19.5f}, STONE);
	addBlock({-3.0f, 0.0f, -3.0f}, {1.0f, 2.6f, -1.5f}, {111, 216, 237, 255});
	addBlock({6.0f, 0.0f, 1.0f}, {8.0f, 2.0f, 5.0f}, {174, 132, 239, 255});
	addBlock({-11.0f, 0.0f, -9.0f}, {-7.0f, 2.3f, -7.0f}, {226, 183, 64, 255});
}

void Game::cast() {
	std::vector<ReagentType> ingredients = coat.getSelected();
	BaseVector base = coat.getBase();
	bool hasAnyReagent = std::any_of(player.getInventory().begin(), player.getInventory().end(), [](int count) { return count > 0; });
	if (!hasAnyReagent && ingredients.empty()) base = BaseVector::Poppet;
	SpellStats spell = CurseVector::build(base, ingredients);
	if (!coat.consume(player.getInventory())) return;
	player.addWard(spell.wardGain);
	castCooldown = base == BaseVector::Poppet ? 0.20f : 0.45f;
	castFlash = 0.18f;
	Vector3 forward = player.getForward();
	Vector3 start = Vector3Add(player.getCamera().position, Vector3Scale(forward, 0.6f));
	if (base == BaseVector::Apple) {
		player.heal(spell.healthGain);
		detonate(player.getPosition(), spell);
		return;
	}
	if (base == BaseVector::Spider) {
		Vector3 location = Vector3Add(player.getPosition(), Vector3Scale(forward, 1.3f));
		location.y = 0.06f;
		mines.push_back({location, spell});
		return;
	}
	float speed = base == BaseVector::Bottle ? 15.0f : 28.0f;
	projectiles.push_back({start, Vector3Scale(forward, speed), spell, base == BaseVector::Bottle ? 2.4f : 1.2f});
}

void Game::detonate(Vector3 position, const SpellStats& spell) {
	for (Enemy& enemy : enemies) {
		if (!enemy.isActive() || enemy.isDead()) continue;
		if (flatDistance(position, enemy.getPosition()) <= spell.radius + enemy.getCollisionRadius()) enemy.applyCurse(spell);
	}
	if (spell.lingerDuration > 0.0f) effects.emplace_back(position, std::max(0.8f, spell.radius), spell.lingerDuration, spell);
}

void Game::updateProjectiles(float deltaTime) {
	for (Projectile& projectile : projectiles) {
		if (projectile.spell.seekRadius > 0.0f) {
			Enemy* target = nullptr;
			float closest = projectile.spell.seekRadius;
			for (Enemy& enemy : enemies) {
				if (!enemy.isActive() || enemy.isDead()) continue;
				float distance = Vector3Distance(projectile.position, enemy.getPosition());
				if (distance < closest) { closest = distance; target = &enemy; }
			}
			if (target != nullptr) {
				Vector3 desired = Vector3Scale(Vector3Normalize(Vector3Subtract(target->getPosition(), projectile.position)), Vector3Length(projectile.velocity));
				projectile.velocity = Vector3Lerp(projectile.velocity, desired, std::min(1.0f, deltaTime * 5.0f));
			}
		}
		if (projectile.spell.vector == BaseVector::Bottle) projectile.velocity.y -= 4.5f * deltaTime;
		projectile.position = Vector3Add(projectile.position, Vector3Scale(projectile.velocity, deltaTime));
		projectile.timeLeft -= deltaTime;
		bool hit = projectile.position.y <= 0.04f || projectile.timeLeft <= 0.0f;
		for (const Enemy& enemy : enemies) {
			if (enemy.isActive() && !enemy.isDead() && Vector3Distance(projectile.position, enemy.getPosition()) < enemy.getCollisionRadius() + 0.25f) hit = true;
		}
		if (hit) {
			detonate(projectile.position, projectile.spell);
			projectile.timeLeft = -1.0f;
		}
	}
	projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& projectile) { return projectile.timeLeft <= 0.0f; }), projectiles.end());
}

void Game::updateMines(float deltaTime) {
	for (Mine& mine : mines) {
		mine.armTime -= deltaTime;
		mine.timeLeft -= deltaTime;
		bool detonateMine = mine.timeLeft <= 0.0f;
		if (mine.armTime <= 0.0f) {
			for (const Enemy& enemy : enemies) {
				if (enemy.isActive() && !enemy.isDead() && flatDistance(mine.position, enemy.getPosition()) < 1.75f) detonateMine = true;
			}
		}
		if (detonateMine) {
			detonate(mine.position, mine.spell);
			mine.timeLeft = -1.0f;
		}
	}
	mines.erase(std::remove_if(mines.begin(), mines.end(), [](const Mine& mine) { return mine.timeLeft <= 0.0f; }), mines.end());
}

void Game::updateEffects(float deltaTime) {
	for (Effect& effect : effects) {
		effect.update(deltaTime);
		for (Enemy& enemy : enemies) {
			if (enemy.isActive() && !enemy.isDead() && flatDistance(effect.getPosition(), enemy.getPosition()) < effect.getRadius()) {
				SpellStats tick = effect.getSpell();
				tick.damage = tick.poisonDamagePerSecond * deltaTime;
				tick.poisonDuration = 0.0f;
				enemy.applyCurse(tick);
			}
		}
	}
	effects.erase(std::remove_if(effects.begin(), effects.end(), [](const Effect& effect) { return !effect.isAlive(); }), effects.end());
}

void Game::updatePickups(float deltaTime) {
	(void)deltaTime;
	for (Pickup& pickup : pickups) {
		pickup.update(time);
		if (pickup.isActive() && flatDistance(player.getPosition(), pickup.getPosition()) < 1.05f) {
			++player.getInventory()[static_cast<int>(pickup.getType())];
			pickup.setActive(false);
		}
	}
	pickups.erase(std::remove_if(pickups.begin(), pickups.end(), [](const Pickup& pickup) { return !pickup.isActive(); }), pickups.end());
}

void Game::drawWorld() const {
	DrawPlane({0.0f, -0.02f, 0.0f}, {39.0f, 39.0f}, GROUND);
	for (int line = -18; line <= 18; line += 2) {
		DrawLine3D({static_cast<float>(line), 0.0f, -19.0f}, {static_cast<float>(line), 0.0f, 19.0f}, Fade(GRID, 0.6f));
		DrawLine3D({-19.0f, 0.0f, static_cast<float>(line)}, {19.0f, 0.0f, static_cast<float>(line)}, Fade(GRID, 0.6f));
	}
	for (const Scenery& object : scenery) object.draw3D();
	for (const Pickup& pickup : pickups) pickup.draw3D();
	for (const Enemy& enemy : enemies) enemy.draw3D();
	for (const Projectile& projectile : projectiles) drawProjectile(projectile);
	for (const Mine& mine : mines) drawMine(mine);
	for (const Effect& effect : effects) effect.draw3D();
}

void Game::drawProjectile(const Projectile& projectile) const {
	Color color = projectile.spell.vector == BaseVector::Poppet ? Color {233, 222, 189, 255} : CURSE;
	DrawSphereWires(projectile.position, 0.16f, 6, 4, color);
	DrawLine3D(Vector3Subtract(projectile.position, Vector3Scale(Vector3Normalize(projectile.velocity), 0.45f)), projectile.position, color);
}

void Game::drawMine(const Mine& mine) const {
	Color color = mine.armTime > 0.0f ? Color {226, 183, 64, 255} : Color {99, 214, 115, 255};
	DrawCircle3D(mine.position, 0.55f, {1.0f, 0.0f, 0.0f}, 90.0f, color);
	DrawSphereWires({mine.position.x, mine.position.y + 0.15f, mine.position.z}, 0.20f, 6, 4, color);
}
