#include <algorithm>
#include <cmath>
#include <sstream>
#include "Game.h"
#include <raymath.h>

namespace {
constexpr float kArenaHalfSize = 18.0f;

float distanceXZ(Vector3 a, Vector3 b) {
	const float dx = a.x - b.x;
	const float dz = a.z - b.z;
	return std::sqrt(dx * dx + dz * dz);
}

Vector3 closestPointOnBoxXZ(const BoundingBox& box, Vector3 point) {
	return {
		std::clamp(point.x, box.min.x, box.max.x),
		point.y,
		std::clamp(point.z, box.min.z, box.max.z)
	};
}
}

Game::Game() : inputHandler(KEY_NULL), player(), coatMenu(), hud(), timeSeconds(0.0f), waveIndex(0) {
	buildLevel();
}

void Game::update() {
	float dt = GetFrameTime();
	dt = std::min(dt, 0.05f);
	InputHandler::InputState input = inputHandler.poll();
	updateWithInput(dt, input);
}

void Game::updateWithInput(float dt, const InputHandler::InputState& input) {
	timeSeconds += dt;

	if (!player.isAlive()) {
		return;
	}

	coatMenu.update(input, player.getInventory());

	Vector3 previousPosition = player.getPosition();
	player.update(dt, input, coatMenu);
	player.applyWorldBounds(-kArenaHalfSize, kArenaHalfSize, -kArenaHalfSize, kArenaHalfSize);
	resolvePlayerCollisions(previousPosition);

	for (Pickup& pickup : pickups) {
		pickup.update(dt);
	}

	handleCasting(input);
	updateEffects(dt);
	updateEnemies(dt);
	collectPickups();

	if (player.isAlive() && enemies.empty()) {
		spawnWave();
	}
}

void Game::draw() const {
	BeginDrawing();
	ClearBackground({8, 9, 14, 255});

	BeginMode3D(player.getCamera());
	DrawPlane({0.0f, 0.0f, 0.0f}, {kArenaHalfSize * 2.0f, kArenaHalfSize * 2.0f}, {28, 31, 35, 255});
	DrawGrid(36, 1.0f);

	for (const Scenery& item : scenery) {
		item.draw3D(player.getCamera());
	}
	for (const Pickup& pickup : pickups) {
		if (pickup.isActive()) {
			pickup.draw3D(player.getCamera());
		}
	}
	for (const Effect& effect : effects) {
		if (effect.isActive()) {
			effect.draw3D(player.getCamera());
		}
	}
	for (const Enemy& enemy : enemies) {
		enemy.draw3D(player.getCamera());
	}

	EndMode3D();

	DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight() / 2, Fade(BLACK, 0.0f), Fade(BLACK, 0.35f));
	hud.draw(*this);

	if (!player.isAlive()) {
		DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));
		DrawText("THE HUNTERS HAVE YOU", GetScreenWidth() / 2 - 180, GetScreenHeight() / 2 - 30, 34, {255, 120, 120, 255});
		DrawText("Close the window to end the run.", GetScreenWidth() / 2 - 140, GetScreenHeight() / 2 + 10, 20, RAYWHITE);
	}

	EndDrawing();
}

const Player& Game::getPlayer() const {
	return player;
}

const CoatMenu& Game::getCoatMenu() const {
	return coatMenu;
}

const std::vector<Enemy>& Game::getEnemies() const {
	return enemies;
}

const std::vector<Pickup>& Game::getPickups() const {
	return pickups;
}

float Game::getTimeSeconds() const {
	return timeSeconds;
}

int Game::getWaveIndex() const {
	return waveIndex;
}

bool Game::runLogicSmoke(std::string& report) {
	std::ostringstream out;
	bool ok = true;

	auto require = [&](bool condition, const char* message) {
		out << (condition ? "[ok] " : "[fail] ") << message << '\n';
		ok = ok && condition;
	};

	require(!pickups.empty(), "world starts with reagent pickups");
	require(!enemies.empty(), "world starts with enemies");

	const Vector3 pickupSpots[] = {
		{-8.0f, 0.0f, 1.5f},
		{8.0f, 0.0f, -1.5f},
		{0.0f, 0.0f, -8.0f},
		{-2.0f, 0.0f, 8.0f},
		{6.5f, 0.0f, 7.0f},
		{-6.5f, 0.0f, -6.0f}
	};

	for (const Vector3& spot : pickupSpots) {
		player.setPosition(spot);
		player.syncCamera();
		updateWithInput(0.016f, {});
	}

	const ReagentInventory& inventory = player.getInventory();
	for (const ReagentInfo& info : getReagentInfos()) {
		require(inventory[getReagentIndex(info.type)] > 0, info.name);
	}

	InputHandler::InputState rummageInput;
	rummageInput.rummageHeld = true;
	rummageInput.basePressed[1] = true;
	rummageInput.reagentPressed[getReagentIndex(ReagentType::GraveSalt)] = true;
	rummageInput.reagentPressed[getReagentIndex(ReagentType::FuneralOil)] = true;
	rummageInput.reagentPressed[getReagentIndex(ReagentType::ChurchGlass)] = true;
	coatMenu.update(rummageInput, player.getInventory());

	require(coatMenu.getRecipe().base == BaseVectorType::Bottle, "rummage can select bottle base");
	require(coatMenu.getRecipe().slotCount == 3, "rummage can select three reagents");

	ComposedSpell spell = coatMenu.getPreview();
	require(spell.damage > 14.0f, "reagents strengthen spell damage");
	require(spell.hazardDuration > 1.0f, "reagents add hazard duration");
	require(spell.splashRadius > 1.0f, "reagents add splash radius");

	Vector3 sprintStart {0.0f, 0.0f, 0.0f};
	player.setPosition(sprintStart);
	player.syncCamera();
	InputHandler::InputState moveInput;
	moveInput.moveInput = {0.0f, 1.0f};
	for (int i = 0; i < 60; ++i) {
		updateWithInput(1.0f / 60.0f, moveInput);
	}
	float freeMoveDistance = Vector3Distance(player.getPosition(), sprintStart);

	player.setPosition(sprintStart);
	player.syncCamera();
	InputHandler::InputState sprintInput = moveInput;
	sprintInput.sprintDown = true;
	for (int i = 0; i < 60; ++i) {
		updateWithInput(1.0f / 60.0f, sprintInput);
	}
	float sprintDistance = Vector3Distance(player.getPosition(), sprintStart);
	require(sprintDistance > freeMoveDistance * 1.2f, "sprint increases movement speed");

	player.setPosition(sprintStart);
	player.syncCamera();
	InputHandler::InputState slowInput = moveInput;
	slowInput.rummageHeld = true;
	for (int i = 0; i < 60; ++i) {
		updateWithInput(1.0f / 60.0f, slowInput);
	}
	float rummageMoveDistance = Vector3Distance(player.getPosition(), sprintStart);
	require(rummageMoveDistance < freeMoveDistance * 0.7f, "rummaging slows movement");

	player.setPosition(sprintStart);
	player.syncCamera();
	InputHandler::InputState jumpInput;
	jumpInput.jumpPressed = true;
	updateWithInput(1.0f / 60.0f, jumpInput);
	float airborneY = player.getFeetY();
	for (int i = 0; i < 90; ++i) {
		updateWithInput(1.0f / 60.0f, {});
	}
	require(airborneY > 0.0f, "jump lifts the player off the ground");
	require(player.isOnGround() && std::abs(player.getFeetY()) < 0.001f, "player lands cleanly after a jump");

	if (!enemies.empty()) {
		enemies.front().setPosition({0.0f, 0.0f, 8.0f});
	}

	player.setPosition({0.0f, 0.0f, 0.0f});
	player.syncCamera();

	const std::size_t startingEnemies = enemies.size();
	float firstEnemyHealthBefore = enemies.empty() ? 0.0f : enemies.front().getHealth();
	InputHandler::InputState castInput;
	castInput.castPressed = true;
	updateWithInput(0.016f, castInput);
	for (int i = 0; i < 180; ++i) {
		updateWithInput(1.0f / 60.0f, {});
	}
	bool enemyCountDropped = enemies.size() < startingEnemies;
	bool enemyWounded = !enemies.empty() && enemies.front().getHealth() < firstEnemyHealthBefore;
	require(enemyCountDropped || enemyWounded, "bottle cast damages or kills enemies");

	enemies.clear();
	effects.clear();
	player.setActive(true);
	player.setHealth(player.getMaxHealth());

	InputHandler::InputState clearInput;
	clearInput.rummageHeld = true;
	clearInput.clearPressed = true;
	coatMenu.update(clearInput, player.getInventory());
	require(coatMenu.getRecipe().slotCount == 0, "coat menu can clear the current mix");

	spawnEnemy({0.0f, 0.0f, 1.5f});
	enemies.front().setPosition({0.0f, 0.0f, 1.2f});
	player.setActive(true);
	player.setHealth(player.getMaxHealth());
	player.spendWard(15.0f);
	player.setPosition({0.0f, 0.0f, 0.0f});
	player.syncCamera();
	for (int i = 0; i < 20; ++i) {
		updateWithInput(1.0f / 60.0f, {});
	}
	float wardBeforeApple = player.getWard();
	float appleEnemyHealth = enemies.front().getHealth();
	const std::size_t appleEnemyCount = enemies.size();
	InputHandler::InputState appleMix;
	appleMix.rummageHeld = true;
	appleMix.basePressed[2] = true;
	appleMix.reagentPressed[getReagentIndex(ReagentType::SaintAsh)] = true;
	coatMenu.update(appleMix, player.getInventory());
	InputHandler::InputState appleCast;
	appleCast.castPressed = true;
	updateWithInput(0.016f, appleCast);
	for (int i = 0; i < 30; ++i) {
		updateWithInput(1.0f / 60.0f, {});
	}
	require(player.getWard() >= wardBeforeApple, "apple cast restores ward");
	bool appleEnemyGone = enemies.size() < appleEnemyCount;
	bool appleEnemyWounded = !enemies.empty() && enemies.front().getHealth() < appleEnemyHealth;
	require(appleEnemyGone || appleEnemyWounded, "apple cast affects nearby enemies");

	enemies.clear();
	effects.clear();
	player.setActive(true);
	player.setHealth(player.getMaxHealth());

	InputHandler::InputState spiderClear;
	spiderClear.rummageHeld = true;
	spiderClear.clearPressed = true;
	coatMenu.update(spiderClear, player.getInventory());
	require(coatMenu.getRecipe().slotCount == 0, "mix can be reset before building a mine");

	InputHandler::InputState spiderMix;
	spiderMix.rummageHeld = true;
	spiderMix.basePressed[3] = true;
	spiderMix.reagentPressed[getReagentIndex(ReagentType::WidowNettle)] = true;
	spiderMix.reagentPressed[getReagentIndex(ReagentType::FuneralOil)] = true;
	coatMenu.update(spiderMix, player.getInventory());
	const std::size_t effectsBeforeMine = effects.size();
	InputHandler::InputState spiderCast;
	spiderCast.castPressed = true;
	updateWithInput(0.016f, spiderCast);
	require(effects.size() > effectsBeforeMine, "spider cast places a mine");
	spawnEnemy(Vector3Add(player.getPosition(), {0.0f, 0.0f, 2.0f}));
	enemies.front().setPosition(Vector3Add(player.getPosition(), {0.0f, 0.0f, 1.6f}));
	float spiderEnemyHealth = enemies.front().getHealth();
	for (int i = 0; i < 180; ++i) {
		updateWithInput(1.0f / 60.0f, {});
	}
	bool spiderEnemyGone = enemies.empty();
	bool spiderEnemyWounded = !enemies.empty() && enemies.front().getHealth() < spiderEnemyHealth;
	require(spiderEnemyGone || spiderEnemyWounded, "spider mine triggers on nearby enemies");

	enemies.clear();
	effects.clear();
	spawnEnemy({0.0f, 0.0f, -3.0f});
	player.setActive(true);
	player.setHealth(player.getMaxHealth());
	player.gainWard(player.getMaxWard());

	player.setPosition(Vector3Add(enemies.front().getPosition(), {0.7f, 0.0f, 0.0f}));
	player.syncCamera();
	float healthBeforeWard = player.getHealth();
	float wardBefore = player.getWard();
	InputHandler::InputState wardInput;
	wardInput.wardDown = true;
	for (int i = 0; i < 40; ++i) {
		updateWithInput(1.0f / 60.0f, wardInput);
	}
	require(player.getWard() < wardBefore, "ward consumes resource while active");
	require(player.getHealth() >= healthBeforeWard - 1.0f, "ward blocks incoming damage");

	float healthBeforeNoWard = player.getHealth();
	for (int i = 0; i < 80; ++i) {
		updateWithInput(1.0f / 60.0f, {});
	}
	require(player.getHealth() < healthBeforeNoWard, "enemy attacks damage player without ward");

	report = out.str();
	return ok;
}

void Game::buildLevel() {
	scenery.clear();
	pickups.clear();
	effects.clear();
	enemies.clear();

	scenery.emplace_back(Vector3 {-14.0f, 0.0f, -12.0f}, Vector3 {6.0f, 4.0f, 5.0f}, SceneryShape::House, true, Color {180, 180, 180, 255});
	scenery.emplace_back(Vector3 {13.0f, 0.0f, -11.0f}, Vector3 {7.0f, 4.5f, 5.0f}, SceneryShape::House, true, Color {180, 180, 180, 255});
	scenery.emplace_back(Vector3 {-13.0f, 0.0f, 11.5f}, Vector3 {7.0f, 4.0f, 4.0f}, SceneryShape::House, true, Color {180, 180, 180, 255});
	scenery.emplace_back(Vector3 {14.5f, 0.0f, 11.5f}, Vector3 {6.0f, 4.0f, 4.0f}, SceneryShape::House, true, Color {180, 180, 180, 255});

	for (int i = -16; i <= 16; i += 4) {
		scenery.emplace_back(Vector3 {static_cast<float>(i), 0.0f, -18.5f}, Vector3 {3.5f, 1.0f, 0.5f}, SceneryShape::Fence, true, Color {140, 140, 140, 255});
		scenery.emplace_back(Vector3 {static_cast<float>(i), 0.0f, 18.5f}, Vector3 {3.5f, 1.0f, 0.5f}, SceneryShape::Fence, true, Color {140, 140, 140, 255});
	}
	for (int i = -16; i <= 16; i += 4) {
		scenery.emplace_back(Vector3 {-18.5f, 0.0f, static_cast<float>(i)}, Vector3 {0.5f, 1.0f, 3.5f}, SceneryShape::Fence, true, Color {140, 140, 140, 255});
		scenery.emplace_back(Vector3 {18.5f, 0.0f, static_cast<float>(i)}, Vector3 {0.5f, 1.0f, 3.5f}, SceneryShape::Fence, true, Color {140, 140, 140, 255});
	}

	scenery.emplace_back(Vector3 {-5.0f, 0.0f, 3.5f}, Vector3 {0.9f, 2.4f, 0.9f}, SceneryShape::Tree, false, Color {190, 190, 190, 255});
	scenery.emplace_back(Vector3 {7.5f, 0.0f, -2.5f}, Vector3 {0.9f, 2.6f, 0.9f}, SceneryShape::Tree, false, Color {190, 190, 190, 255});
	scenery.emplace_back(Vector3 {-2.5f, 0.0f, -7.5f}, Vector3 {1.0f, 1.2f, 1.0f}, SceneryShape::Stone, true, Color {170, 170, 170, 255});
	scenery.emplace_back(Vector3 {4.5f, 0.0f, 7.0f}, Vector3 {1.3f, 1.1f, 1.3f}, SceneryShape::Stone, true, Color {170, 170, 170, 255});

	spawnPickup({-8.0f, 0.5f, 1.5f}, ReagentType::GraveSalt, 2);
	spawnPickup({8.0f, 0.5f, -1.5f}, ReagentType::SaintAsh, 2);
	spawnPickup({0.0f, 0.5f, -8.0f}, ReagentType::WidowNettle, 2);
	spawnPickup({-2.0f, 0.5f, 8.0f}, ReagentType::FuneralOil, 2);
	spawnPickup({6.5f, 0.5f, 7.0f}, ReagentType::MothDust, 2);
	spawnPickup({-6.5f, 0.5f, -6.0f}, ReagentType::ChurchGlass, 2);

	spawnWave();
}

void Game::handleCasting(const InputHandler::InputState& input) {
	if (!input.castPressed || !player.isAlive() || player.getCastCooldown() > 0.0f) {
		return;
	}

	const SpellRecipe& recipe = coatMenu.getRecipe();
	ComposedSpell spell = coatMenu.getPreview();

	if (!spell.fallbackPoppet && !player.consumeReagents(recipe)) {
		return;
	}

	player.triggerCastFlash();

	Vector3 origin = player.getCamera().position;
	Vector3 forward = Vector3Normalize(Vector3Subtract(player.getCamera().target, player.getCamera().position));

	switch (spell.base) {
		case BaseVectorType::Poppet:
		case BaseVectorType::Bottle:
			effects.push_back(Effect::makeProjectile(Vector3Add(origin, Vector3Scale(forward, 0.8f)), forward, spell));
			break;
		case BaseVectorType::Apple:
			player.gainWard(spell.wardGain);
			effects.push_back(Effect::makeSelfPulse(player.getPosition(), spell));
			applySpellBurst(spell, player.getPosition(), 1.0f);
			break;
		case BaseVectorType::Spider:
			effects.push_back(Effect::makeMine(Vector3Add(player.getPosition(), Vector3Scale(player.getForward(), 1.4f)), spell));
			break;
	}
}

void Game::resolvePlayerCollisions(Vector3 previousPosition) {
	for (const Scenery& item : scenery) {
		if (!item.isCollidable()) {
			continue;
		}

		BoundingBox box = item.getBoundingBox();
		Vector3 closest = closestPointOnBoxXZ(box, player.getPosition());
		Vector3 delta = Vector3Subtract(player.getPosition(), closest);
		delta.y = 0.0f;
		float distSq = Vector3LengthSqr(delta);
		float radius = player.getCollisionRadius();

		if (distSq < radius * radius) {
			if (distSq < 0.0001f) {
				player.setPosition(previousPosition);
				player.syncCamera();
				return;
			}

			float dist = std::sqrt(distSq);
			Vector3 push = Vector3Scale(delta, (radius - dist) / dist);
			player.setPosition(Vector3Add(player.getPosition(), push));
		}
	}

	player.syncCamera();
}

void Game::collectPickups() {
	for (Pickup& pickup : pickups) {
		if (!pickup.isActive()) {
			continue;
		}

		if (distanceXZ(pickup.getPosition(), player.getPosition()) < 1.0f) {
			player.addReagent(pickup.getType(), pickup.getAmount());
			pickup.setActive(false);
		}
	}

	pickups.erase(
		std::remove_if(pickups.begin(), pickups.end(), [](const Pickup& pickup) { return !pickup.isActive(); }),
		pickups.end()
	);
}

void Game::updateEffects(float dt) {
	for (Effect& effect : effects) {
		if (!effect.isActive()) {
			continue;
		}

		if (effect.getMode() == EffectMode::Projectile && effect.getSpell().homingStrength > 0.0f && !enemies.empty()) {
			Enemy* closestEnemy = nullptr;
			float closestDistance = 9999.0f;
			for (Enemy& enemy : enemies) {
				if (!enemy.isAlive()) {
					continue;
				}
				float dist = distanceXZ(enemy.getPosition(), effect.getPosition());
				if (dist < closestDistance) {
					closestDistance = dist;
					closestEnemy = &enemy;
				}
			}
			if (closestEnemy != nullptr) {
				Vector3 seek = Vector3Normalize(Vector3Subtract(closestEnemy->getPosition(), effect.getPosition()));
				Vector3 newVelocity = Vector3Lerp(effect.getVelocity(), Vector3Scale(seek, effect.getSpell().projectileSpeed), effect.getSpell().homingStrength * dt * 2.0f);
				effect.setVelocity(newVelocity);
			}
		}

		effect.update(dt);
		if (!effect.isActive()) {
			continue;
		}

		bool burst = false;
		if (effect.getMode() == EffectMode::Projectile) {
			if (effect.getPosition().y <= 0.05f) {
				burst = true;
			}

			for (const Scenery& item : scenery) {
				if (item.isCollidable() && CheckCollisionBoxSphere(item.getBoundingBox(), effect.getPosition(), effect.getCollisionRadius())) {
					burst = true;
					break;
				}
			}

			for (Enemy& enemy : enemies) {
				if (enemy.isAlive() && distanceXZ(enemy.getPosition(), effect.getPosition()) <= effect.getCollisionRadius() + enemy.getCollisionRadius()) {
					burst = true;
					break;
				}
			}

			if (burst) {
				applySpellBurst(effect.getSpell(), effect.getPosition(), 1.0f);
				if (effect.getSpell().hazardDuration > 0.2f) {
					effects.push_back(Effect::makeHazard(effect.getPosition(), effect.getSpell()));
				}
				effect.setActive(false);
			}
		} else if (effect.getMode() == EffectMode::Mine) {
			for (Enemy& enemy : enemies) {
				if (enemy.isAlive() && distanceXZ(enemy.getPosition(), effect.getPosition()) <= effect.getTriggerRadius()) {
					applySpellBurst(effect.getSpell(), effect.getPosition(), 1.0f);
					if (effect.getSpell().hazardDuration > 0.2f) {
						effects.push_back(Effect::makeHazard(effect.getPosition(), effect.getSpell()));
					}
					effect.setActive(false);
					break;
				}
			}
		} else if (effect.getMode() == EffectMode::Hazard) {
			if (effect.getTickTimer() <= 0.0f) {
				applySpellBurst(effect.getSpell(), effect.getPosition(), 0.25f);
				effect.setTickTimer(0.35f);
			}
		}
	}

	effects.erase(
		std::remove_if(effects.begin(), effects.end(), [](const Effect& effect) { return !effect.isActive(); }),
		effects.end()
	);
}

void Game::updateEnemies(float dt) {
	for (Enemy& enemy : enemies) {
		enemy.updateAI(dt, player.getPosition());
		if (enemy.canAttack(player.getPosition())) {
			player.takeHit(enemy.consumeAttack());
		}
	}

	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive() && !enemy.hasDroppedLoot()) {
			enemy.markLootDropped();
			ReagentType type = static_cast<ReagentType>(GetRandomValue(0, static_cast<int>(ReagentType::Count) - 1));
			spawnPickup(enemy.getPosition(), type, GetRandomValue(1, 2));
		}
	}

	enemies.erase(
		std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) { return !enemy.isAlive(); }),
		enemies.end()
	);
}

void Game::spawnWave() {
	++waveIndex;
	const int enemyCount = 4 + waveIndex * 2;
	for (int i = 0; i < enemyCount; ++i) {
		float side = (i % 2 == 0) ? -1.0f : 1.0f;
		float row = static_cast<float>(i / 2);
		spawnEnemy({side * (11.0f + row * 1.3f), 0.0f, -10.0f + row * 3.4f});
	}
}

void Game::spawnEnemy(Vector3 position) {
	enemies.emplace_back(position);
}

void Game::spawnPickup(Vector3 position, ReagentType type, int amount) {
	pickups.emplace_back(position, type, amount);
}

void Game::applySpellBurst(const ComposedSpell& spell, const Vector3& center, float radiusScale) {
	const float radius = std::max(0.5f, spell.splashRadius * radiusScale);
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) {
			continue;
		}
		float dist = distanceXZ(center, enemy.getPosition());
		if (dist <= radius) {
			float falloff = 1.0f - std::min(1.0f, dist / std::max(0.1f, radius));
			enemy.applySpell(spell, center, 0.4f + falloff * 0.6f);
			if (spell.wardGain > 0.0f && spell.base != BaseVectorType::Apple) {
				player.gainWard(spell.wardGain * 0.4f * falloff);
			}
		}
	}
}
