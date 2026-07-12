#include "Game.h"

#include <raymath.h>

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {
constexpr Color INK{21, 29, 30, 255};
constexpr Color BONE{226, 232, 207, 255};
constexpr Color MOSS{102, 206, 145, 255};
constexpr Color HURT{238, 98, 69, 255};

float flatDistance(Vector3 a, Vector3 b) { a.y = b.y = 0.0f; return Vector3Distance(a, b); }
Color reagentColor(ReagentType type) {
	Color colors[] = {{210, 220, 195, 255}, {74, 193, 104, 255}, {202, 193, 93, 255}, {194, 117, 42, 255}, {240, 238, 213, 255}, {184, 136, 62, 255}};
	return colors[static_cast<int>(type)];
}
}

Game::Game(int screenWidth, int screenHeight) : width(screenWidth), height(screenHeight), input(KEY_NULL) {
	for (int i = -16; i <= 16; i += 8) obstacles.push_back({{static_cast<float>(i) - 2.5f, 0.0f, -17.35f}, {static_cast<float>(i) + 2.5f, 4.0f, -16.65f}});
	obstacles.push_back({{-6.0f, 0.0f, -1.0f}, {-3.5f, 2.5f, 1.5f}});
	obstacles.push_back({{5.0f, 0.0f, -5.5f}, {7.5f, 2.5f, -3.0f}});
	for (int i = 0; i < 6; ++i) pickups.push_back({{-9.0f + i * 3.6f, 0.35f, 4.0f - (i % 2) * 8.0f}, static_cast<ReagentType>(i)});
	for (int i = 0; i < 9; ++i) enemies.push_back({{-11.0f + (i % 5) * 5.2f, 0.7f, -8.0f - (i / 5) * 6.0f}});
	for (int i = 0; i < 5; ++i) enemies.push_back({{10.0f - i * 3.5f, 0.7f, 5.0f + (i % 2) * 4.0f}});
}

void Game::cast() {
	if (castCooldown > 0.0f || coat.isOpen()) return;
	CurseVector spell = coat.spell();
	spell.rebuild();
	if (spell.base == CurseBase::Apple) applyApple(spell);
	else {
		Vector3 origin = Vector3Add(player.getCamera().position, Vector3Scale(Vector3Normalize(Vector3Subtract(player.getCamera().target, player.getCamera().position)), 0.7f));
		if (spell.base == CurseBase::Spider) {
			projectiles.push_back({{origin.x, 0.16f, origin.z}, {}, spell, spell.duration > 0.0f ? spell.duration : 12.0f, true});
			std::snprintf(message, sizeof(message), "A widow's trap unfolds.");
		} else {
			Vector3 direction = Vector3Normalize(Vector3Subtract(player.getCamera().target, player.getCamera().position));
			projectiles.push_back({origin, Vector3Scale(direction, spell.base == CurseBase::Bottle ? 16.0f : 25.0f), spell});
		}
	}
	for (int i = 0; i < spell.reagentCount; ++i) {
		int& count = inventory[static_cast<int>(spell.reagents[i])];
		if (count > 0) --count;
	}
	coat.clearSpell();
	castCooldown = 0.22f;
}

void Game::applyApple(const CurseVector& spell) {
	player.damage(-(18.0f + spell.damage * 0.35f));
	if (spell.wardGain) player.restoreWard(26.0f);
	// An eaten omen bursts outward, letting curse effects retain their normal meaning.
	detonate(spell, player.position);
	std::snprintf(message, sizeof(message), "You bite the omen: the curse blooms outward.");
	messageTime = 2.0f;
}

void Game::detonate(const CurseVector& spell, Vector3 position) {
	float radius = std::max(0.9f, spell.radius);
	for (Enemy& enemy : enemies) {
		if (enemy.dead || flatDistance(enemy.position, position) > radius) continue;
		enemy.health -= spell.damage;
		enemy.poisonTimer = spell.poison ? 4.0f + spell.duration : 0.0f;
		enemy.snareTimer = spell.snare ? 2.0f + spell.duration : 0.0f;
		enemy.fearTimer = spell.fear ? 2.0f + spell.duration : 0.0f;
		if (spell.wardGain) player.damage(-16.0f);
		if (enemy.health <= 0.0f) { enemy.dead = true; pickups.push_back({enemy.position, static_cast<ReagentType>(GetRandomValue(0, 5))}); }
	}
}

void Game::update(float dt) {
	InputHandler::InputState controls = input.poll();
	coat.update(inventory);
	if (IsKeyPressed(KEY_E)) player.toggleWard();
	player.update(controls, dt, coat.isOpen());
	player.constrain(20.0f);
	player.resolveCollisions(obstacles);
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) cast();
	castCooldown = std::max(0.0f, castCooldown - dt);
	messageTime = std::max(0.0f, messageTime - dt);
	for (Pickup& pickup : pickups) if (!pickup.collected && flatDistance(pickup.position, player.position) < 1.2f) { pickup.collected = true; ++inventory[static_cast<int>(pickup.type)]; std::snprintf(message, sizeof(message), "Found %s", reagentName(pickup.type)); messageTime = 1.8f; }
	for (Enemy& enemy : enemies) {
		if (enemy.dead) continue;
		enemy.poisonTimer = std::max(0.0f, enemy.poisonTimer - dt);
		enemy.snareTimer = std::max(0.0f, enemy.snareTimer - dt);
		enemy.fearTimer = std::max(0.0f, enemy.fearTimer - dt);
		if (enemy.poisonTimer > 0.0f) enemy.health -= 5.0f * dt;
		if (enemy.health <= 0.0f) { enemy.dead = true; pickups.push_back({enemy.position, static_cast<ReagentType>(GetRandomValue(0, 5))}); continue; }
		float dist = flatDistance(enemy.position, player.position);
		if (dist < 14.0f && enemy.fearTimer <= 0.0f && enemy.snareTimer <= 0.0f) {
			Vector3 toward = Vector3Normalize(Vector3Subtract(player.position, enemy.position));
			enemy.position = Vector3Add(enemy.position, Vector3Scale(toward, dt * 1.3f));
		}
		enemy.attackTimer -= dt;
		if (dist < 1.35f && enemy.attackTimer <= 0.0f) { player.damage(9.0f); enemy.attackTimer = 1.1f; }
	}
	for (Projectile& projectile : projectiles) {
		projectile.life -= dt;
		if (!projectile.mine) {
			if (projectile.spell.seeking) {
				const Enemy* nearest = nullptr;
				float nearestDistance = 12.0f;
				for (const Enemy& enemy : enemies) {
					float distance = flatDistance(enemy.position, projectile.position);
					if (!enemy.dead && distance < nearestDistance) { nearest = &enemy; nearestDistance = distance; }
				}
				if (nearest) {
					Vector3 desired = Vector3Scale(Vector3Normalize(Vector3Subtract(nearest->position, projectile.position)), Vector3Length(projectile.velocity));
					projectile.velocity = Vector3Lerp(projectile.velocity, desired, std::min(1.0f, dt * 6.0f));
				}
			}
			projectile.position = Vector3Add(projectile.position, Vector3Scale(projectile.velocity, dt));
		}
		bool hit = false;
		for (const Enemy& enemy : enemies) if (!enemy.dead && flatDistance(enemy.position, projectile.position) < (projectile.mine ? 1.7f : 0.75f)) hit = true;
		if (hit || projectile.life <= 0.0f) { detonate(projectile.spell, projectile.position); projectile.life = -1.0f; }
	}
	projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& item) { return item.life < 0.0f; }), projectiles.end());
}

void Game::drawWorld() const {
	ClearBackground({37, 48, 48, 255});
	BeginMode3D(player.getCamera());
	DrawPlane({0, 0, 0}, {45.0f, 45.0f}, {50, 65, 57, 255});
	DrawGrid(20, 2.0f);
	for (int i = -16; i <= 16; i += 8) {
		DrawCube({static_cast<float>(i), 2.0f, -17.0f}, 5.0f, 4.0f, 0.7f, INK);
		DrawCubeWires({static_cast<float>(i), 2.0f, -17.0f}, 5.0f, 4.0f, 0.7f, BONE);
	}
	for (const BoundingBox& obstacle : obstacles) {
		if (obstacle.min.z < -15.0f) continue;
		Vector3 center{(obstacle.min.x + obstacle.max.x) * 0.5f, 1.25f, (obstacle.min.z + obstacle.max.z) * 0.5f};
		DrawCube(center, obstacle.max.x - obstacle.min.x, 2.5f, obstacle.max.z - obstacle.min.z, INK);
		DrawCubeWires(center, obstacle.max.x - obstacle.min.x, 2.5f, obstacle.max.z - obstacle.min.z, BONE);
	}
	for (const Pickup& pickup : pickups) if (!pickup.collected) {
		DrawSphere({pickup.position.x, 0.38f + sinf(GetTime() * 3.0f + pickup.position.x) * 0.1f, pickup.position.z}, 0.22f, reagentColor(pickup.type));
		DrawSphereWires({pickup.position.x, 0.38f, pickup.position.z}, 0.25f, 6, 6, BONE);
	}
	for (const Enemy& enemy : enemies) {
		Color color = enemy.dead ? Color{74, 63, 55, 255} : enemy.poisonTimer > 0.0f ? Color{104, 192, 91, 255} : HURT;
		DrawCube(enemy.position, 0.82f, 1.4f, 0.14f, color);
		DrawCubeWires(enemy.position, 0.84f, 1.42f, 0.16f, BONE);
		if (!enemy.dead) DrawLine3D({enemy.position.x - 0.45f, 1.55f, enemy.position.z}, {enemy.position.x - 0.45f + enemy.health / 60.0f, 1.55f, enemy.position.z}, MOSS);
	}
	for (const Projectile& projectile : projectiles) {
		Color color = projectile.mine ? Color{192, 89, 121, 255} : Color{223, 204, 99, 255};
		DrawSphere(projectile.position, projectile.mine ? 0.36f : 0.18f, color);
		DrawSphereWires(projectile.position, projectile.mine ? 0.4f : 0.22f, 8, 8, BONE);
	}
	if (player.isWardActive()) DrawSphereWires(player.position, 1.15f, 12, 8, {104, 219, 202, 255});
	EndMode3D();
}

void Game::drawHud() const {
	int w = GetScreenWidth(), h = GetScreenHeight();
	DrawRectangle(18, 18, 242, 62, Fade(INK, 0.82f));
	DrawRectangleLines(18, 18, 242, 62, BONE);
	DrawText("FLESH", 30, 29, 15, BONE); DrawRectangle(91, 31, 150, 13, {60, 66, 61, 255}); DrawRectangle(91, 31, static_cast<int>(player.getHealth() * 1.5f), 13, HURT);
	DrawText("WARD", 30, 54, 15, BONE); DrawRectangle(91, 56, 150, 13, {60, 66, 61, 255}); DrawRectangle(91, 56, static_cast<int>(player.getWard() * 1.5f), 13, player.isWardActive() ? Color{90, 218, 201, 255} : Color{91, 133, 138, 255});
	DrawText("[E] WARD", 275, 26, 16, player.isWardActive() ? Color{90, 218, 201, 255} : BONE);
	DrawText("HOLD [TAB] OR RMB: RUMMAGE     LMB: CAST     SHIFT: SPRINT", 18, h - 29, 15, BONE);
	DrawText("+", w / 2 - 7, h / 2 - 12, 26, BONE);
	// The foreground hands keep the witch embodied even while casting and rummaging.
	DrawTriangle({w * 0.17f, static_cast<float>(h)}, {w * 0.38f, h * 0.74f}, {w * 0.47f, static_cast<float>(h)}, {46, 57, 53, 255});
	DrawTriangle({w * 0.83f, static_cast<float>(h)}, {w * 0.62f, h * 0.74f}, {w * 0.53f, static_cast<float>(h)}, {46, 57, 53, 255});
	DrawLineEx({w * 0.17f, static_cast<float>(h)}, {w * 0.38f, h * 0.74f}, 3.0f, BONE);
	DrawLineEx({w * 0.83f, static_cast<float>(h)}, {w * 0.62f, h * 0.74f}, 3.0f, BONE);
	if (messageTime > 0.0f) DrawText(message, w / 2 - MeasureText(message, 18) / 2, 96, 18, {240, 211, 123, 255});
	if (coat.isOpen()) coat.draw(w, h, inventory, 0.45f);
}

void Game::draw() const { drawWorld(); drawHud(); }
