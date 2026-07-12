#include "Game.h"

#include <raymath.h>
#include <algorithm>
#include <cmath>

namespace {
float FlatDistance(Vector3 a, Vector3 b) {
	a.y = 0.0f;
	b.y = 0.0f;
	return Vector3Distance(a, b);
}
}

Game::Game() : input(KEY_NULL) {
	DisableCursor();
	reset();
}

void Game::reset() {
	player.reset();
	coat.clear();
	enemies.clear();
	pickups.clear();
	projectiles.clear();
	hazards.clear();
	scenery.clear();
	scenery.emplace_back(BoundingBox{{-5.5f, 0.0f, -2.5f}, {-3.5f, 2.5f, 2.5f}});
	scenery.emplace_back(BoundingBox{{3.5f, 0.0f, -2.5f}, {5.5f, 2.5f, 2.5f}});
	scenery.emplace_back(BoundingBox{{-1.5f, 0.0f, -7.5f}, {1.5f, 1.2f, -6.5f}});
	wave = 1;
	for (int i = 0; i < 8; ++i) {
		const float angle = i * PI * 0.25f;
		enemies.emplace_back(Vector3{std::sin(angle) * (8.0f + (i % 2) * 4.0f), 0.0f, std::cos(angle) * (8.0f + (i % 2) * 4.0f)}, i > 5 ? 1 : 0);
	}
	for (int i = 0; i < ReagentCount; ++i) {
		pickups.emplace_back(Vector3{-7.5f + i * 3.0f, 0.0f, 1.5f}, static_cast<ReagentType>(i));
	}
	message = "Find reagents. Build a hex. Survive Salem.";
	messageTime = 5.0f;
}

void Game::resolveWorldCollision(Vector3 previousPosition) {
	const float radius = 0.38f;
	for (const Scenery& object : scenery) {
		const BoundingBox& box = object.bounds();
		const BoundingBox playerBox {{player.position.x - radius, 0.0f, player.position.z - radius},
			{player.position.x + radius, 1.75f, player.position.z + radius}};
		if (!CheckCollisionBoxes(playerBox, box)) continue;
		player.position.x = previousPosition.x;
		const BoundingBox xBox {{player.position.x - radius, 0.0f, player.position.z - radius},
			{player.position.x + radius, 1.75f, player.position.z + radius}};
		if (CheckCollisionBoxes(xBox, box)) player.position.z = previousPosition.z;
	}
}

void Game::say(const char* text) {
	message = text;
	messageTime = 2.0f;
}

void Game::castSpell() {
	if (castCooldown > 0.0f || coat.open) return;
	const SpellStats stats = coat.spell.stats();
	if (coat.spell.type == VectorType::Apple) {
		player.health = std::min(player.maxHealth, player.health + 14.0f + stats.poisonDamage);
		player.ward = std::min(player.maxWard, player.ward + stats.wardGain);
		explode(player.position, stats);
		say("You bite the bitter apple. The curse turns inward.");
	} else if (coat.spell.type == VectorType::Spider) {
		Vector3 p = Vector3Add(player.position, Vector3Scale(player.forward(), 1.2f));
		p.y = 0.05f;
		hazards.emplace_back(p, stats, std::max(5.0f, stats.duration));
		say("A patient spider waits in the dirt.");
	} else {
		const float speed = coat.spell.type == VectorType::Poppet ? 24.0f : 13.0f;
		projectiles.push_back({Vector3Add(player.eyePosition(), Vector3Scale(player.forward(), 0.5f)), Vector3Scale(player.forward(), speed), stats, coat.spell.type, 3.0f});
	}
	castCooldown = coat.spell.type == VectorType::Poppet ? 0.32f : 0.55f;
	coat.spell.reagentSlots = 0;
}

void Game::explode(Vector3 position, const SpellStats& stats, bool hazard) {
	for (Enemy& enemy : enemies) {
		if (!enemy.dead() && FlatDistance(enemy.position, position) <= std::max(0.7f, stats.radius)) {
			enemy.hurt(stats.damage, stats.poisonDamage, stats.snareSeconds, stats.fearSeconds);
			player.ward = std::min(player.maxWard, player.ward + stats.wardGain);
		}
	}
	if (!hazard && stats.duration > 0.0f) hazards.emplace_back(Vector3{position.x, 0.05f, position.z}, stats, stats.duration);
	screenFlash = 0.13f;
}

void Game::updateCombat(float dt) {
	const float healthBefore = player.health;
	const float wardBefore = player.ward;
	for (Enemy& enemy : enemies) if (!enemy.dead()) enemy.update(dt, player);
	if (player.health < healthBefore || player.ward < wardBefore - dt * 25.0f) {
		screenFlash = 0.18f;
		say(player.wardActive ? "The ward catches a hunter's blow." : "A hunter tears your flesh.");
	}
	for (Projectile& projectile : projectiles) {
		projectile.life -= dt;
		if (projectile.stats.homing > 0.0f) {
			Enemy* nearest = nullptr;
			float distance = 12.0f;
			for (Enemy& enemy : enemies) {
				const float d = Vector3Distance(projectile.position, enemy.position);
				if (!enemy.dead() && d < distance) { nearest = &enemy; distance = d; }
			}
			if (nearest) {
				Vector3 desired = Vector3Scale(Vector3Normalize(Vector3Subtract(Vector3Add(nearest->position, {0, 0.7f, 0}), projectile.position)), Vector3Length(projectile.velocity));
				projectile.velocity = Vector3Lerp(projectile.velocity, desired, std::min(1.0f, projectile.stats.homing * dt));
			}
		}
		if (projectile.type == VectorType::Bottle) projectile.velocity.y -= 5.5f * dt;
		projectile.position = Vector3Add(projectile.position, Vector3Scale(projectile.velocity, dt));
		for (Enemy& enemy : enemies) {
			if (!enemy.dead() && Vector3Distance(projectile.position, Vector3Add(enemy.position, {0, 0.65f, 0})) < 0.75f) {
				explode(projectile.position, projectile.stats);
				projectile.life = 0.0f;
				break;
			}
		}
		if (projectile.position.y < 0.0f) {
			explode(projectile.position, projectile.stats);
			projectile.life = 0.0f;
		}
	}
	projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& p) { return p.life <= 0.0f; }), projectiles.end());
	for (Effect& hazard : hazards) {
		hazard.life -= dt;
		hazard.tick -= dt;
		if (hazard.tick <= 0.0f) {
			SpellStats tickStats = hazard.stats;
			tickStats.damage = std::max(4.0f, hazard.stats.damage * 0.35f);
			explode(hazard.position, tickStats, true);
			hazard.tick = 0.7f;
		}
	}
	hazards.erase(std::remove_if(hazards.begin(), hazards.end(), [](const Effect& h) { return h.life <= 0.0f; }), hazards.end());
	for (Enemy& enemy : enemies) {
		if (enemy.dead() && !enemy.dropped) {
			enemy.dropped = true;
			pickups.emplace_back(enemy.position, static_cast<ReagentType>(GetRandomValue(0, ReagentCount - 1)));
		}
	}
	for (Pickup& pickup : pickups) {
		pickup.update(dt);
		if (pickup.active && FlatDistance(pickup.position, player.position) < 0.9f) {
			player.inventory[static_cast<int>(pickup.type)] += GetRandomValue(1, 2);
			pickup.active = false;
			say(GetReagentInfo(pickup.type).name);
		}
	}
}

void Game::update() {
	float dt = std::min(GetFrameTime(), 0.05f);
	const InputHandler::InputState state = input.poll();
	coat.open = IsKeyDown(KEY_R);
	if (coat.open) coat.update(player);
	const Vector3 previousPosition = player.position;
	player.update(dt, state, coat.open);
	resolveWorldCollision(previousPosition);
	player.camera.position = player.eyePosition();
	player.camera.target = Vector3Add(player.camera.position, player.forward());
	castCooldown = std::max(0.0f, castCooldown - dt);
	screenFlash = std::max(0.0f, screenFlash - dt);
	messageTime = std::max(0.0f, messageTime - dt);
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) castSpell();
	if (player.health > 0.0f) updateCombat(dt);
	if (IsKeyPressed(KEY_ENTER) && (player.health <= 0.0f || std::none_of(enemies.begin(), enemies.end(), [](const Enemy& e) { return !e.dead(); }))) reset();
}

void Game::drawWorld() const {
	DrawPlane({0, 0, 0}, {38, 38}, Color{32, 38, 31, 255});
	for (int i = -18; i <= 18; i += 2) {
		DrawLine3D({static_cast<float>(i), 0.01f, -18}, {static_cast<float>(i), 0.01f, 18}, Color{47, 59, 46, 255});
		DrawLine3D({-18, 0.01f, static_cast<float>(i)}, {18, 0.01f, static_cast<float>(i)}, Color{47, 59, 46, 255});
	}
	for (int side : {-1, 1}) {
		for (int i = -15; i <= 15; i += 6) {
			Vector3 p{side * 18.8f, 1.5f, static_cast<float>(i)};
			DrawCubeWires(p, 1.2f, 3.0f, 4.2f, Color{229, 205, 137, 255});
			DrawLine3D({p.x - 0.6f, 3.0f, p.z - 2.1f}, {p.x + 0.6f, 4.0f, p.z}, Color{229, 205, 137, 255});
			DrawLine3D({p.x + 0.6f, 4.0f, p.z}, {p.x - 0.6f, 3.0f, p.z + 2.1f}, Color{229, 205, 137, 255});
		}
	}
	for (const Scenery& object : scenery) object.draw();
	for (const Pickup& pickup : pickups) if (pickup.active) pickup.draw();
	for (const Enemy& enemy : enemies) if (!enemy.dead()) enemy.draw();
	for (const Projectile& p : projectiles) {
		DrawSphere(p.position, p.type == VectorType::Poppet ? 0.12f : 0.22f, Color{235, 86, 68, 255});
		DrawLine3D(p.position, Vector3Subtract(p.position, Vector3Scale(Vector3Normalize(p.velocity), 0.8f)), Color{229, 205, 137, 255});
	}
	for (const Effect& h : hazards) {
		DrawCircle3D(h.position, std::max(0.7f, h.stats.radius), {1, 0, 0}, 90.0f, Color{114, 205, 92, 255});
		for (int i = 0; i < 8; ++i) {
			const float a = i * PI / 4.0f;
			DrawLine3D(h.position, {h.position.x + std::sin(a) * h.stats.radius, 0.4f, h.position.z + std::cos(a) * h.stats.radius}, Color{114, 205, 92, 255});
		}
	}
}

void Game::drawHands() const {
	const int w = GetScreenWidth();
	const int h = GetScreenHeight();
	Color ink = player.wardActive ? Color{91, 188, 207, 255} : Color{229, 205, 137, 255};
	DrawTriangle({w * 0.18f, static_cast<float>(h)}, {w * 0.34f, h * 0.69f}, {w * 0.42f, static_cast<float>(h)}, Fade(BLACK, 0.75f));
	DrawTriangleLines({w * 0.18f, static_cast<float>(h)}, {w * 0.34f, h * 0.69f}, {w * 0.42f, static_cast<float>(h)}, ink);
	DrawTriangle({w * 0.82f, static_cast<float>(h)}, {w * 0.66f, h * 0.69f}, {w * 0.58f, static_cast<float>(h)}, Fade(BLACK, 0.75f));
	DrawTriangleLines({w * 0.82f, static_cast<float>(h)}, {w * 0.66f, h * 0.69f}, {w * 0.58f, static_cast<float>(h)}, ink);
	for (int i = 0; i < 4; ++i) {
		DrawLine(w * 0.34f, h * 0.69f, w * (0.27f + i * 0.025f), h * (0.59f - std::abs(i - 1) * 0.025f), ink);
		DrawLine(w * 0.66f, h * 0.69f, w * (0.73f - i * 0.025f), h * (0.59f - std::abs(i - 1) * 0.025f), ink);
	}
}

void Game::draw() const {
	BeginDrawing();
	ClearBackground(Color{11, 15, 15, 255});
	BeginMode3D(player.camera);
	drawWorld();
	EndMode3D();
	drawHands();
	if (player.wardActive) {
		DrawCircleLines(GetScreenWidth() / 2, GetScreenHeight() / 2, GetScreenHeight() * 0.32f, Color{91, 188, 207, 220});
		DrawCircleLines(GetScreenWidth() / 2, GetScreenHeight() / 2, GetScreenHeight() * 0.325f, Fade(SKYBLUE, 0.45f));
	}
	const int living = static_cast<int>(std::count_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return !e.dead(); }));
	hud.draw(player, coat, living, messageTime, message);
	if (coat.open) coat.draw(player);
	if (screenFlash > 0.0f) DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(Color{235, 86, 68, 255}, screenFlash * 2.0f));
	if (player.health <= 0.0f || living == 0) {
		DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.72f));
		const char* title = player.health <= 0.0f ? "SALEM TOOK THE WITCH" : "THE ROAD OUT IS OPEN";
		DrawText(title, GetScreenWidth() / 2 - MeasureText(title, 38) / 2, GetScreenHeight() / 2 - 35, 38, player.health <= 0.0f ? RED : Color{229, 205, 137, 255});
		DrawText("PRESS ENTER TO BEGIN AGAIN", GetScreenWidth() / 2 - 162, GetScreenHeight() / 2 + 22, 20, RAYWHITE);
	}
	EndDrawing();
}
