#include "Game.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

#include <raymath.h>

namespace {
constexpr Color BACKGROUND_COLOR = {5, 7, 11, 255};
constexpr Color GROUND_COLOR = {10, 15, 17, 255};
constexpr Color GRID_COLOR = {34, 56, 55, 150};
constexpr Color CURSE_COLOR = {193, 255, 111, 255};
constexpr Color HUNTER_SHOT_COLOR = {255, 104, 74, 255};

float horizontalDistance(Vector3 a, Vector3 b) {
	const float x = a.x - b.x;
	const float z = a.z - b.z;
	return std::sqrt(x * x + z * z);
}

BoundingBox expandBox(BoundingBox box, float amount) {
	box.min.x -= amount;
	box.min.y -= amount;
	box.min.z -= amount;
	box.max.x += amount;
	box.max.y += amount;
	box.max.z += amount;
	return box;
}

bool groundSegmentHit(Vector3 start, Vector3 end, float radius, float& distance, Vector3& point) {
	if (start.y <= radius) {
		distance = 0.0f;
		point = start;
		point.y = radius;
		return true;
	}
	if (end.y > radius || end.y == start.y) {
		return false;
	}
	const float fraction = std::clamp((start.y - radius) / (start.y - end.y), 0.0f, 1.0f);
	point = Vector3Lerp(start, end, fraction);
	point.y = radius;
	distance = Vector3Distance(start, point);
	return true;
}

bool segmentBlocked(Vector3 start, Vector3 end, const std::vector<BoundingBox>& boxes,
		float padding = 0.0f) {
	const Vector3 travel = Vector3Subtract(end, start);
	const float distance = Vector3Length(travel);
	if (distance <= 0.0001f) return false;
	const Ray ray {start, Vector3Scale(travel, 1.0f / distance)};
	for (const BoundingBox& box : boxes) {
		const RayCollision collision = GetRayCollisionBox(ray, expandBox(box, padding));
		if (collision.hit && collision.distance < distance - 0.001f) {
			return true;
		}
	}
	return false;
}
}

Game::Game()
	: inputHandler(KEY_NULL) {
	assets.load();
	buildArena();
	resetRun();
	state = State::Title;
}

Game::~Game() = default;

void Game::update() {
	float deltaTime = std::min(GetFrameTime(), 0.05f);
	elapsedTime += deltaTime;
	InputHandler::InputState input = inputHandler.poll();

	if (IsKeyPressed(KEY_F11)) {
		ToggleFullscreen();
	}

	if (state == State::Title) {
		if (mainMenu.updateAndCheckStart()) {
			resetRun();
			state = State::Playing;
			inputHandler.setMouseCaptured(true);
		}
		return;
	}

	if (state == State::Defeat) {
		if (input.restartPressed) {
			resetRun();
			state = State::Playing;
			inputHandler.setMouseCaptured(true);
		}
		return;
	}

	coatMenu.update(input, player);
	if (coatMenu.wasCommitted()) {
		hud.notify(player.getPreparedSpell().getCurseName() + " READIED", CURSE_COLOR);
	}

	player.updateFromInput(deltaTime, input, getPlayerSolidBounds(), coatMenu.isActive());
	hud.update(deltaTime);

	if (input.castPressed && !coatMenu.isActive() && inputHandler.isMouseCaptured()) {
		castPreparedSpell();
	}

	updateEnemies(deltaTime);
	updateProjectiles(deltaTime);
	updateMines(deltaTime);
	updateEffects(deltaTime);
	updatePickups(deltaTime);
	handleEnemyDeaths();

	if (enemies.empty()) {
		if (nextWaveTime <= 0.0f) {
			nextWaveTime = 3.0f;
			hud.notify("THE ROAD QUIETS // NEXT HUNT IN 3", {238, 208, 138, 255});
		} else {
			nextWaveTime -= deltaTime;
			if (nextWaveTime <= 0.0f) {
				spawnWave();
			}
		}
	} else {
		nextWaveTime = 0.0f;
	}

	if (!player.isAlive()) {
		state = State::Defeat;
		coatMenu.reset();
		inputHandler.setMouseCaptured(false);
	}
}

void Game::draw() const {
	BeginDrawing();
	ClearBackground(BACKGROUND_COLOR);
	drawWorld();

	if (state == State::Title) {
		mainMenu.draw();
	} else if (state == State::Playing) {
		int aliveEnemies = 0;
		for (const Enemy& enemy : enemies) {
			if (enemy.isAlive()) {
				++aliveEnemies;
			}
		}
		if (coatMenu.isActive()) {
			coatMenu.draw(player, assets);
		}
		hud.draw(player, assets, wave, aliveEnemies, coatMenu.isActive(),
			inputHandler.isMouseCaptured());
	}

	if (state == State::Defeat) {
		const int width = GetScreenWidth();
		const int height = GetScreenHeight();
		DrawRectangle(0, 0, width, height, {35, 4, 8, 185});
		const char* title = "SALEM TOOK YOUR NAME";
		const int titleSize = std::max(28, width / 28);
		DrawText(title, width / 2 - MeasureText(title, titleSize) / 2, height / 2 - 54,
			titleSize, {255, 112, 96, 255});
		const char* prompt = "ENTER  rise and try the road again";
		DrawText(prompt, width / 2 - MeasureText(prompt, 20) / 2, height / 2 + 18, 20,
			{238, 224, 190, 255});
	}

	EndDrawing();
}

void Game::resetRun() {
	enemies.clear();
	pickups.clear();
	effects.clear();
	playerProjectiles.clear();
	hostileProjectiles.clear();
	mines.clear();
	coatMenu.reset();
	hud.reset();
	player.reset({0.0f, 0.88f, 13.5f});
	wave = 0;
	nextWaveTime = 0.0f;
	spawnStartingPickups();
	spawnWave();
	hud.notify("POPPET READY // HOLD TAB TO RUMMAGE", {238, 224, 190, 255});
}

void Game::buildArena() {
	scenery.clear();
	const Color wall = {109, 140, 124, 255};
	const Color timber = {160, 119, 83, 255};
	const Color iron = {108, 168, 178, 255};

	// Arena bounds.
	scenery.emplace_back(SceneryKind::Fence, Vector3 {0.0f, 1.5f, -20.5f},
		Vector3 {42.0f, 3.0f, 0.7f}, wall, true);
	scenery.emplace_back(SceneryKind::Fence, Vector3 {0.0f, 1.5f, 20.5f},
		Vector3 {42.0f, 3.0f, 0.7f}, wall, true);
	scenery.emplace_back(SceneryKind::Fence, Vector3 {-20.5f, 1.5f, 0.0f},
		Vector3 {0.7f, 3.0f, 42.0f}, wall, true);
	scenery.emplace_back(SceneryKind::Fence, Vector3 {20.5f, 1.5f, 0.0f},
		Vector3 {0.7f, 3.0f, 42.0f}, wall, true);

	// Blocky hostile Salem cover, intentionally readable rather than ornate.
	scenery.emplace_back(SceneryKind::SolidBox, Vector3 {-10.5f, 2.0f, -7.0f},
		Vector3 {5.0f, 4.0f, 4.0f}, timber, true);
	scenery.emplace_back(SceneryKind::SolidBox, Vector3 {10.0f, 1.65f, -2.0f},
		Vector3 {5.5f, 3.3f, 3.5f}, timber, true);
	scenery.emplace_back(SceneryKind::SolidBox, Vector3 {-4.0f, 1.15f, 4.0f},
		Vector3 {2.5f, 2.3f, 2.5f}, iron, true);
	scenery.emplace_back(SceneryKind::SolidBox, Vector3 {5.0f, 1.0f, 8.0f},
		Vector3 {3.0f, 2.0f, 1.8f}, iron, true);

	// Non-solid vector props establish the road and skyline.
	scenery.emplace_back(SceneryKind::Gallows, Vector3 {0.0f, 3.0f, -17.5f},
		Vector3 {4.0f, 6.0f, 1.0f}, Color {190, 103, 72, 255}, false);
	scenery.emplace_back(SceneryKind::DeadTree, Vector3 {-16.5f, 3.5f, -12.0f},
		Vector3 {3.0f, 7.0f, 3.0f}, wall, false);
	scenery.emplace_back(SceneryKind::DeadTree, Vector3 {16.0f, 3.5f, 11.0f},
		Vector3 {3.0f, 7.0f, 3.0f}, wall, false);
	scenery.emplace_back(SceneryKind::Lantern, Vector3 {-7.0f, 2.0f, 10.0f},
		Vector3 {1.0f, 4.0f, 1.0f}, Color {255, 171, 71, 255}, false);
	scenery.emplace_back(SceneryKind::Lantern, Vector3 {7.0f, 2.0f, 1.0f},
		Vector3 {1.0f, 4.0f, 1.0f}, Color {255, 171, 71, 255}, false);
	for (int i = 0; i < 5; ++i) {
		const float stoneHeight = 1.4f + static_cast<float>(i % 2) * 0.6f;
		scenery.emplace_back(SceneryKind::StandingStone,
			Vector3 {-14.0f + static_cast<float>(i) * 2.2f, stoneHeight * 0.5f, 15.5f},
			Vector3 {0.8f, stoneHeight, 0.6f},
			Color {92, 116, 113, 255}, false);
	}
}

void Game::spawnStartingPickups() {
	const std::array<Vector3, REAGENT_TYPE_COUNT> positions = {{
		{-6.0f, 0.65f, 10.0f}, {-3.6f, 0.65f, 8.5f}, {-1.2f, 0.65f, 10.0f},
		{1.2f, 0.65f, 10.0f}, {3.6f, 0.65f, 8.5f}, {6.0f, 0.65f, 10.0f}
	}};
	for (std::size_t i = 0; i < REAGENT_TYPE_COUNT; ++i) {
		const Texture2D* icon = assets.reagentIcons[i].id != 0 ? &assets.reagentIcons[i] : nullptr;
		pickups.emplace_back(static_cast<ReagentType>(i), 2, positions[i], icon);
	}
}

void Game::spawnWave() {
	++wave;
	const std::array<Vector3, 10> spawnPoints = {{
		{0.0f, 0.9f, -15.0f}, {-7.0f, 0.9f, -13.5f}, {8.0f, 0.9f, -12.0f},
		{-16.0f, 0.9f, -2.0f}, {16.0f, 0.9f, 5.0f}, {-15.0f, 0.9f, 10.0f},
		{14.0f, 0.9f, -9.0f}, {-3.0f, 0.9f, -3.0f}, {12.0f, 0.9f, 15.0f},
		{-11.0f, 0.9f, 16.0f}
	}};
	const int requestedCount = std::min(9, 3 + wave);
	const std::vector<BoundingBox> solids = getSolidBounds();
	int spawnedCount = 0;
	for (int i = 0; i < requestedCount; ++i) {
		const bool hunter = wave > 1 && ((i + wave) % 3 == 0);
		const EnemyKind kind = hunter ? EnemyKind::WitchHunter : EnemyKind::TorchBearer;
		const Texture2D* sheet = nullptr;
		if (kind == EnemyKind::WitchHunter && assets.witchHunterSheet.id != 0) {
			sheet = &assets.witchHunterSheet;
		} else if (kind == EnemyKind::TorchBearer && assets.torchBearerSheet.id != 0) {
			sheet = &assets.torchBearerSheet;
		}
		const Vector3 enemySize = kind == EnemyKind::TorchBearer
			? Vector3 {0.82f, 1.75f, 0.82f} : Vector3 {0.9f, 1.9f, 0.9f};
		for (std::size_t attempt = 0; attempt < spawnPoints.size(); ++attempt) {
			const std::size_t pointIndex = static_cast<std::size_t>(i + wave - 1
				+ static_cast<int>(attempt)) % spawnPoints.size();
			Vector3 candidate = spawnPoints[pointIndex];
			candidate.y = enemySize.y * 0.5f;
			if (horizontalDistance(candidate, player.getPosition()) < 8.0f) continue;
			const Vector3 halfSize = Vector3Scale(enemySize, 0.5f);
			const BoundingBox candidateBounds {
				Vector3Subtract(candidate, halfSize), Vector3Add(candidate, halfSize)
			};
			bool blocked = false;
			for (const BoundingBox& box : solids) {
				if (CheckCollisionBoxes(candidateBounds, box)) {
					blocked = true;
					break;
				}
			}
			if (blocked) continue;
			for (const Enemy& enemy : enemies) {
				if (enemy.isAlive() && CheckCollisionBoxes(candidateBounds, enemy.getBounds())) {
					blocked = true;
					break;
				}
			}
			if (blocked) continue;
			enemies.emplace_back(kind, candidate,
				static_cast<ReagentType>((i + wave) % static_cast<int>(REAGENT_TYPE_COUNT)), sheet);
			++spawnedCount;
			break;
		}
	}
	hud.notify("HUNT " + std::to_string(wave) + " // " + std::to_string(spawnedCount) + " SOULS CLOSE IN",
		{255, 132, 91, 255});
}

void Game::castPreparedSpell() {
	if (!player.canCast()) {
		return;
	}

	const CurseVector& curse = player.getPreparedSpell();
	if (curse.getType() == CurseVectorType::Poppet) {
		castPoppet();
		return;
	}
	if (!curse.canAfford(player.getInventory())) {
		hud.notify("REAGENTS SPENT // POPPET FALLBACK", {255, 184, 92, 255});
		castPoppet();
		return;
	}

	const SpellStats stats = curse.calculateStats();
	Vector3 spiderPlacement {};
	if (curse.getType() == CurseVectorType::Spider && !findSpiderPlacement(spiderPlacement)) {
		hud.notify("NO GROUND FOR THE SPIDER", {255, 104, 74, 255});
		player.beginCastCooldown(0.2f);
		return;
	}
	if (!player.consumePreparedReagents()) {
		return;
	}
	player.beginCastCooldown(stats.cooldown);
	hud.triggerCast();

	switch (curse.getType()) {
	case CurseVectorType::Bottle: {
		PlayerProjectile projectile;
		projectile.position = Vector3Add(player.getEyePosition(),
			Vector3Scale(player.getLookDirection(), 0.55f));
		projectile.previousPosition = projectile.position;
		projectile.velocity = Vector3Add(Vector3Scale(player.getLookDirection(), stats.projectileSpeed),
			Vector3 {0.0f, 1.5f, 0.0f});
		projectile.stats = stats;
		playerProjectiles.push_back(projectile);
		break;
	}
	case CurseVectorType::Apple:
		player.heal(stats.selfHeal);
		player.applyHaste(stats.hasteDuration);
		resolveSpell(player.getPosition(), stats);
		break;
	case CurseVectorType::Spider: {
		SpiderMine mine;
		mine.position = spiderPlacement;
		mine.stats = stats;
		mines.push_back(mine);
		effects.emplace_back(EffectKind::SnareSigil, mine.position,
			std::max(0.75f, stats.radius * 0.65f), 0.55f, Color {156, 225, 95, 255}, stats);
		break;
	}
	case CurseVectorType::Poppet:
		break;
	}
}

void Game::castPoppet() {
	const SpellStats stats = CurseVector(CurseVectorType::Poppet).calculateStats();
	player.beginCastCooldown(stats.cooldown);
	hud.triggerCast();

	Ray ray {player.getEyePosition(), player.getLookDirection()};
	Enemy* target = nullptr;
	float targetDistance = 36.0f;
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) {
			continue;
		}
		const RayCollision collision = GetRayCollisionBox(ray, enemy.getBounds());
		if (collision.hit && collision.distance < targetDistance) {
			target = &enemy;
			targetDistance = collision.distance;
		}
	}

	for (const Scenery& prop : scenery) {
		if (!prop.isCollidable()) {
			continue;
		}
		const RayCollision collision = GetRayCollisionBox(ray, prop.getBounds());
		if (collision.hit && collision.distance < targetDistance) {
			target = nullptr;
			targetDistance = collision.distance;
		}
	}

	const Vector3 end = Vector3Add(ray.position, Vector3Scale(ray.direction, targetDistance));
	effects.emplace_back(EffectKind::HitSpark, end, 0.25f, 0.18f,
		target != nullptr ? CURSE_COLOR : Color {91, 132, 121, 255});
	if (target != nullptr) {
		target->takeDamage(stats.impactDamage);
		hud.triggerHit();
	}
}

void Game::resolveSpell(Vector3 impactPosition, const SpellStats& stats, Enemy* directTarget) {
	const float radius = std::max(0.6f, stats.radius);
	const Vector3 floorPosition {impactPosition.x, 0.04f, impactPosition.z};
	bool hit = false;
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) {
			continue;
		}
		const float distance = horizontalDistance(impactPosition, enemy.getPosition());
		if (&enemy != directTarget && distance > radius) {
			continue;
		}

		float damage = stats.impactDamage;
		if (&enemy != directTarget && radius > 0.0f) {
			damage *= std::clamp(1.0f - distance / (radius * 1.8f), 0.45f, 1.0f);
		}
		if (damage > 0.0f) {
			enemy.takeDamage(damage);
		}
		if (stats.snareDuration > 0.0f) {
			enemy.applySnare(stats.snareDuration);
		}
		if (stats.rotDuration > 0.0f) {
			enemy.applyRot(stats.rotDamagePerSecond, stats.rotDuration);
		}
		if (stats.fearDuration > 0.0f) {
			enemy.applyFear(stats.fearDuration);
		}
		hit = true;
	}

	if (stats.wardGain > 0.0f) {
		player.gainWard(stats.wardGain);
		effects.emplace_back(EffectKind::WardPulse, player.getPosition(), 1.2f, 0.5f,
			Color {91, 209, 255, 255}, stats);
	}
	effects.emplace_back(EffectKind::Burst, impactPosition, radius, 0.42f, CURSE_COLOR, stats);
	if (stats.snareDuration > 0.0f) {
		effects.emplace_back(EffectKind::SnareSigil, floorPosition, radius, 0.9f,
			Color {158, 218, 91, 255}, stats);
	}
	if (stats.fearDuration > 0.0f) {
		effects.emplace_back(EffectKind::FearPulse, floorPosition, radius, 0.65f,
			Color {220, 112, 255, 255}, stats);
	}
	if (stats.lingeringDuration > 0.0f && stats.rotDamagePerSecond > 0.0f) {
		effects.emplace_back(EffectKind::LingeringRot, floorPosition, radius,
			stats.lingeringDuration, Color {116, 199, 74, 210}, stats);
	}
	if (hit) {
		hud.triggerHit();
	}
}

void Game::updateProjectiles(float deltaTime) {
	const std::vector<BoundingBox> solids = getSolidBounds();
	for (PlayerProjectile& projectile : playerProjectiles) {
		if (!projectile.active) {
			continue;
		}
		projectile.life -= deltaTime;
		projectile.previousPosition = projectile.position;
		if (projectile.stats.seekingStrength > 0.0f) {
			Enemy* target = findNearestEnemy(projectile.position,
				7.0f + projectile.stats.seekingStrength * 2.0f);
			if (target != nullptr) {
				const float speed = std::max(1.0f, Vector3Length(projectile.velocity));
				const Vector3 desired = Vector3Normalize(Vector3Subtract(target->getPosition(), projectile.position));
				const Vector3 current = Vector3Normalize(projectile.velocity);
				const float turn = std::clamp(projectile.stats.seekingStrength * 2.4f * deltaTime, 0.0f, 1.0f);
				projectile.velocity = Vector3Scale(Vector3Normalize(Vector3Lerp(current, desired, turn)), speed);
			}
		}
		projectile.velocity.y -= 5.5f * deltaTime;
		projectile.position = Vector3Add(projectile.position,
			Vector3Scale(projectile.velocity, deltaTime));

		const Vector3 travel = Vector3Subtract(projectile.position, projectile.previousPosition);
		const float travelDistance = Vector3Length(travel);
		Enemy* directTarget = nullptr;
		bool struck = false;
		float nearestDistance = travelDistance + 0.001f;
		Vector3 impactPosition = projectile.position;
		if (travelDistance > 0.0001f) {
			const Ray ray {projectile.previousPosition, Vector3Scale(travel, 1.0f / travelDistance)};
			for (Enemy& enemy : enemies) {
				if (!enemy.isAlive()) continue;
				const RayCollision collision = GetRayCollisionBox(ray,
					expandBox(enemy.getBounds(), projectile.radius));
				if (collision.hit && collision.distance <= travelDistance
						&& collision.distance < nearestDistance) {
					struck = true;
					nearestDistance = collision.distance;
					impactPosition = collision.point;
					directTarget = &enemy;
				}
			}
			for (const BoundingBox& box : solids) {
				const RayCollision collision = GetRayCollisionBox(ray, expandBox(box, projectile.radius));
				if (collision.hit && collision.distance <= travelDistance
						&& collision.distance < nearestDistance) {
					struck = true;
					nearestDistance = collision.distance;
					impactPosition = collision.point;
					directTarget = nullptr;
				}
			}
			float groundDistance = 0.0f;
			Vector3 groundPoint {};
			if (groundSegmentHit(projectile.previousPosition, projectile.position,
					projectile.radius, groundDistance, groundPoint)
					&& groundDistance < nearestDistance) {
				struck = true;
				impactPosition = groundPoint;
				directTarget = nullptr;
			}
		}

		if (struck || projectile.life <= 0.0f) {
			projectile.position = impactPosition;
			resolveSpell(impactPosition, projectile.stats, directTarget);
			projectile.active = false;
		}
	}

	for (HostileProjectile& projectile : hostileProjectiles) {
		if (!projectile.active) {
			continue;
		}
		projectile.life -= deltaTime;
		const Vector3 previousPosition = projectile.position;
		projectile.position = Vector3Add(projectile.position,
			Vector3Scale(projectile.velocity, deltaTime));

		const Vector3 travel = Vector3Subtract(projectile.position, previousPosition);
		const float travelDistance = Vector3Length(travel);
		bool struck = false;
		bool struckPlayer = false;
		float nearestDistance = travelDistance + 0.001f;
		Vector3 impactPosition = projectile.position;
		if (travelDistance > 0.0001f) {
			const Ray ray {previousPosition, Vector3Scale(travel, 1.0f / travelDistance)};
			const RayCollision playerCollision = GetRayCollisionSphere(ray, player.getPosition(), 0.66f);
			if (playerCollision.hit && playerCollision.distance <= travelDistance) {
				struck = true;
				struckPlayer = true;
				nearestDistance = playerCollision.distance;
				impactPosition = playerCollision.point;
			}
			for (const BoundingBox& box : solids) {
				const RayCollision collision = GetRayCollisionBox(ray, expandBox(box, 0.18f));
				if (collision.hit && collision.distance <= travelDistance
						&& collision.distance < nearestDistance) {
					struck = true;
					struckPlayer = false;
					nearestDistance = collision.distance;
					impactPosition = collision.point;
				}
			}
			float groundDistance = 0.0f;
			Vector3 groundPoint {};
			if (groundSegmentHit(previousPosition, projectile.position, 0.18f,
					groundDistance, groundPoint) && groundDistance < nearestDistance) {
				struck = true;
				struckPlayer = false;
				impactPosition = groundPoint;
			}
		}

		if (struckPlayer) {
			player.takeDamage(projectile.damage);
			effects.emplace_back(EffectKind::HitSpark, impactPosition, 0.35f, 0.24f,
				HUNTER_SHOT_COLOR);
		}
		if (struck) {
			projectile.position = impactPosition;
			projectile.active = false;
		}
		if (projectile.life <= 0.0f) {
			projectile.active = false;
		}
	}

	playerProjectiles.erase(std::remove_if(playerProjectiles.begin(), playerProjectiles.end(),
		[](const PlayerProjectile& projectile) { return !projectile.active; }), playerProjectiles.end());
	hostileProjectiles.erase(std::remove_if(hostileProjectiles.begin(), hostileProjectiles.end(),
		[](const HostileProjectile& projectile) { return !projectile.active; }), hostileProjectiles.end());
}

void Game::updateMines(float deltaTime) {
	for (SpiderMine& mine : mines) {
		if (!mine.active) {
			continue;
		}
		mine.armTime -= deltaTime;
		mine.life -= deltaTime;
		const float triggerRadius = 1.1f + mine.stats.radius * 0.35f
			+ mine.stats.seekingStrength * 0.8f;
		if (mine.armTime <= 0.0f) {
			Enemy* target = findNearestEnemy(mine.position, triggerRadius);
			if (target != nullptr) {
				resolveSpell(mine.position, mine.stats, target);
				mine.active = false;
			}
		}
		if (mine.life <= 0.0f) {
			mine.active = false;
		}
	}
	mines.erase(std::remove_if(mines.begin(), mines.end(),
		[](const SpiderMine& mine) { return !mine.active; }), mines.end());
}

void Game::updateEffects(float deltaTime) {
	for (Effect& effect : effects) {
		effect.update(deltaTime);
		if (effect.getKind() == EffectKind::LingeringRot && effect.takePulse()) {
			const SpellStats& stats = effect.getSpellStats();
			for (Enemy& enemy : enemies) {
				if (enemy.isAlive() && effect.contains(enemy.getPosition())) {
					enemy.takeDamage(stats.rotDamagePerSecond * 0.35f);
				}
			}
		}
	}
	effects.erase(std::remove_if(effects.begin(), effects.end(),
		[](const Effect& effect) { return !effect.isActive(); }), effects.end());
}

void Game::updateEnemies(float deltaTime) {
	const std::vector<BoundingBox> solids = getSolidBounds();
	for (std::size_t index = 0; index < enemies.size(); ++index) {
		Enemy& enemy = enemies[index];
		if (!enemy.isAlive()) {
			continue;
		}
		std::vector<BoundingBox> actorSolids = solids;
		actorSolids.reserve(solids.size() + enemies.size());
		actorSolids.push_back(player.getBounds());
		for (std::size_t otherIndex = 0; otherIndex < enemies.size(); ++otherIndex) {
			if (otherIndex != index && enemies[otherIndex].isAlive()) {
				actorSolids.push_back(enemies[otherIndex].getBounds());
			}
		}
		const EnemyAttackEvent attack = enemy.updateAI(deltaTime, player.getPosition(), actorSolids);
		if (!attack.valid) {
			continue;
		}
		if (attack.projectile) {
			HostileProjectile projectile;
			projectile.position = attack.origin;
			projectile.velocity = Vector3Scale(Vector3Normalize(attack.direction), 10.0f);
			projectile.damage = attack.damage;
			hostileProjectiles.push_back(projectile);
		} else if (horizontalDistance(enemy.getPosition(), player.getPosition()) < 2.3f) {
			Vector3 playerTarget = player.getPosition();
			playerTarget.y += 0.2f;
			if (!segmentBlocked(attack.origin, playerTarget, solids)) {
				player.takeDamage(attack.damage);
			}
		}
	}
}

void Game::updatePickups(float deltaTime) {
	for (Pickup& pickup : pickups) {
		if (!pickup.isActive()) {
			continue;
		}
		pickup.update(deltaTime);
		if (pickup.canCollect(player.getPosition())) {
			const int accepted = player.addReagent(pickup.getReagentType(), pickup.getAmount());
			const int collected = pickup.takeAmount(accepted);
			if (collected > 0) {
				const ReagentDefinition& definition = getReagentDefinition(pickup.getReagentType());
				hud.notify(std::string(definition.name) + "  +" + std::to_string(collected)
					+ " // HOLD TAB", definition.color);
			}
		}
	}
	pickups.erase(std::remove_if(pickups.begin(), pickups.end(),
		[](const Pickup& pickup) { return !pickup.isActive(); }), pickups.end());
}

void Game::handleEnemyDeaths() {
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive() && enemy.claimDrop()) {
			const std::size_t typeIndex = static_cast<std::size_t>(enemy.getDropType());
			const Texture2D* icon = assets.reagentIcons[typeIndex].id != 0
				? &assets.reagentIcons[typeIndex] : nullptr;
			Vector3 dropPosition = enemy.getPosition();
			dropPosition.y = 0.65f;
			pickups.emplace_back(enemy.getDropType(), wave >= 3 ? 2 : 1, dropPosition, icon);
		}
	}
	enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
		[](const Enemy& enemy) { return !enemy.isAlive(); }), enemies.end());
}

void Game::drawWorld() const {
	BeginMode3D(player.getCamera());
	DrawPlane({0.0f, -0.02f, 0.0f}, {40.0f, 40.0f}, GROUND_COLOR);
	for (int line = -20; line <= 20; line += 2) {
		const Color color = line == 0 ? Color {61, 91, 83, 190} : GRID_COLOR;
		DrawLine3D({static_cast<float>(line), 0.0f, -20.0f},
			{static_cast<float>(line), 0.0f, 20.0f}, color);
		DrawLine3D({-20.0f, 0.0f, static_cast<float>(line)},
			{20.0f, 0.0f, static_cast<float>(line)}, color);
	}

	for (const Scenery& prop : scenery) {
		prop.draw3D(player.getCamera());
	}
	for (const Pickup& pickup : pickups) {
		pickup.draw3D(player.getCamera());
	}
	for (const Effect& effect : effects) {
		effect.draw3D(player.getCamera());
	}
	for (const Enemy& enemy : enemies) {
		enemy.draw3D(player.getCamera());
	}
	drawProjectiles();
	EndMode3D();
}

void Game::drawProjectiles() const {
	for (const PlayerProjectile& projectile : playerProjectiles) {
		DrawLine3D(projectile.previousPosition, projectile.position, Fade(CURSE_COLOR, 0.55f));
		DrawSphereWires(projectile.position, 0.18f, 5, 7, CURSE_COLOR);
	}
	for (const HostileProjectile& projectile : hostileProjectiles) {
		DrawSphereWires(projectile.position, 0.17f, 4, 6, HUNTER_SHOT_COLOR);
		DrawLine3D(projectile.position,
			Vector3Subtract(projectile.position, Vector3Scale(Vector3Normalize(projectile.velocity), 0.8f)),
			Fade(HUNTER_SHOT_COLOR, 0.65f));
	}
	for (const SpiderMine& mine : mines) {
		const Color color = mine.armTime > 0.0f ? Color {111, 131, 92, 255} : CURSE_COLOR;
		DrawCircle3D(mine.position, 0.42f, {1.0f, 0.0f, 0.0f}, 90.0f, color);
		for (int leg = 0; leg < 8; ++leg) {
			const float angle = static_cast<float>(leg) * PI / 4.0f;
			const Vector3 inner = {mine.position.x + std::cos(angle) * 0.16f, mine.position.y,
				mine.position.z + std::sin(angle) * 0.16f};
			const Vector3 outer = {mine.position.x + std::cos(angle) * 0.5f, mine.position.y,
				mine.position.z + std::sin(angle) * 0.5f};
			DrawLine3D(inner, outer, color);
		}
	}
}

bool Game::findSpiderPlacement(Vector3& placement) const {
	const std::vector<BoundingBox> solids = getSolidBounds();
	const Vector3 forward = player.getFlatForward();
	const std::array<float, 4> distances = {1.5f, 1.05f, 0.6f, 0.0f};
	for (const float distance : distances) {
		Vector3 candidate = Vector3Add(player.getPosition(), Vector3Scale(forward, distance));
		candidate.y = 0.08f;
		bool overlaps = false;
		for (const BoundingBox& box : solids) {
			if (CheckCollisionBoxSphere(box, candidate, 0.28f)) {
				overlaps = true;
				break;
			}
		}
		if (overlaps) continue;

		const Vector3 pathStart {player.getPosition().x, 0.32f, player.getPosition().z};
		const Vector3 pathEnd {candidate.x, 0.32f, candidate.z};
		if (segmentBlocked(pathStart, pathEnd, solids, 0.28f)) continue;
		placement = candidate;
		return true;
	}
	return false;
}

std::vector<BoundingBox> Game::getSolidBounds() const {
	std::vector<BoundingBox> result;
	result.reserve(scenery.size());
	for (const Scenery& prop : scenery) {
		if (prop.isActive() && prop.isCollidable()) {
			result.push_back(prop.getBounds());
		}
	}
	return result;
}

std::vector<BoundingBox> Game::getPlayerSolidBounds() const {
	std::vector<BoundingBox> result = getSolidBounds();
	result.reserve(result.size() + enemies.size());
	for (const Enemy& enemy : enemies) {
		if (enemy.isAlive()) {
			// Keep bodies tangible without allowing a melee ring to imprison the player.
			const Vector3 centre = enemy.getPosition();
			const float radius = enemy.getCollisionRadius() * 0.64f;
			BoundingBox blocker = enemy.getBounds();
			blocker.min.x = centre.x - radius;
			blocker.max.x = centre.x + radius;
			blocker.min.z = centre.z - radius;
			blocker.max.z = centre.z + radius;
			result.push_back(blocker);
		}
	}
	return result;
}

Enemy* Game::findNearestEnemy(Vector3 searchPosition, float maxDistance) {
	Enemy* result = nullptr;
	float nearest = maxDistance;
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) {
			continue;
		}
		const float distance = Vector3Distance(searchPosition, enemy.getPosition());
		if (distance < nearest) {
			nearest = distance;
			result = &enemy;
		}
	}
	return result;
}
