#include "Game.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <raymath.h>

namespace {
float flatDistance(Vector3 a, Vector3 b) {
	a.y = 0.0f;
	b.y = 0.0f;
	return Vector3Distance(a, b);
}

Vector3 groundPoint(Vector3 value) {
	return Vector3{value.x, 0.06f, value.z};
}

bool pointInBounds(Vector3 point, BoundingBox bounds) {
	return point.x >= bounds.min.x && point.x <= bounds.max.x &&
		point.y >= bounds.min.y && point.y <= bounds.max.y &&
		point.z >= bounds.min.z && point.z <= bounds.max.z;
}
}

Game::Game() : input(KEY_NULL) {
	reset();
	input.setMouseCaptured(false);
}

void Game::reset() {
	player.reset();
	coat.setOpen(false);
	scenery.clear();
	enemies.clear();
	pickups.clear();
	effects.clear();
	collected = 0;
	elapsed = 0.0f;

	const Color structure{105, 135, 125, 255};
	const Color fence{95, 105, 90, 255};
	scenery.emplace_back(Vector3{-12.5f, 2.5f, -5.0f}, Vector3{6.0f, 5.0f, 7.0f}, structure);
	scenery.emplace_back(Vector3{12.5f, 2.0f, -14.0f}, Vector3{7.0f, 4.0f, 6.0f}, structure);
	scenery.emplace_back(Vector3{-13.5f, 2.2f, -24.0f}, Vector3{5.0f, 4.4f, 6.0f}, structure);
	scenery.emplace_back(Vector3{0.0f, 1.0f, -9.0f}, Vector3{3.5f, 2.0f, 1.2f}, fence);
	scenery.emplace_back(Vector3{-5.0f, 0.8f, -19.0f}, Vector3{1.2f, 1.6f, 5.0f}, fence);
	scenery.emplace_back(Vector3{7.5f, 0.75f, -27.0f}, Vector3{5.0f, 1.5f, 1.0f}, fence);
	scenery.emplace_back(Vector3{-20.0f, 1.5f, -13.0f}, Vector3{1.0f, 3.0f, 42.0f}, fence);
	scenery.emplace_back(Vector3{20.0f, 1.5f, -13.0f}, Vector3{1.0f, 3.0f, 42.0f}, fence);
	scenery.emplace_back(Vector3{0.0f, 1.5f, 13.0f}, Vector3{40.0f, 3.0f, 1.0f}, fence);
	scenery.emplace_back(Vector3{0.0f, 1.5f, -39.0f}, Vector3{40.0f, 3.0f, 1.0f}, fence);

	const Vector3 enemyPositions[] = {
		{-4.0f, 0.0f, -2.0f}, {5.0f, 0.0f, -7.0f}, {-8.0f, 0.0f, -13.0f},
		{7.0f, 0.0f, -18.0f}, {1.0f, 0.0f, -24.0f}, {-9.0f, 0.0f, -31.0f},
		{11.0f, 0.0f, -33.0f}
	};
	for (int i = 0; i < 7; ++i) enemies.emplace_back(enemyPositions[i], i % 3);

	const Vector3 pickupPositions[] = {
		{-2.0f, 0.35f, 4.0f}, {3.5f, 0.35f, 0.0f}, {-7.0f, 0.35f, -7.0f},
		{8.0f, 0.35f, -11.0f}, {-2.0f, 0.35f, -17.0f}, {4.0f, 0.35f, -29.0f}
	};
	for (int i = 0; i < REAGENT_COUNT; ++i) pickups.emplace_back(pickupPositions[i], static_cast<ReagentType>(i), 2);
	setMessage("ESCAPE SALEM // COLLECT, RUMMAGE, HEX", 4.0f);
}

std::vector<BoundingBox> Game::obstacleBounds() const {
	std::vector<BoundingBox> bounds;
	for (const Scenery& object : scenery) {
		if (object.isSolid()) bounds.push_back(object.getBounds());
	}
	return bounds;
}

void Game::update(float dt) {
	dt = std::min(dt, 0.05f);
	elapsed += dt;
	messageTimer = std::max(0.0f, messageTimer - dt);
	const InputHandler::InputState state = input.poll();
	if (!started) {
		mainMenu.update();
		if (mainMenu.takePlayRequest()) {
			started = true;
			input.setMouseCaptured(true);
			setMessage("ESCAPE SALEM // COLLECT, RUMMAGE, HEX", 4.0f);
		}
		return;
	}

	if (!player.isAlive()) {
		if (IsKeyPressed(KEY_R)) reset();
		return;
	}
	const bool anyAliveBeforeUpdate = std::any_of(enemies.begin(), enemies.end(), [](const Enemy& enemy) { return enemy.isAlive(); });
	if (!anyAliveBeforeUpdate && IsKeyPressed(KEY_R)) {
		reset();
		return;
	}

	coat.setOpen(state.rummageDown);
	coat.update(player);
	player.update(dt, state, obstacleBounds(), coat.isOpen());
	collectPickups();
	updateEnemies(dt);
	updateEffects(dt);
	if (state.castPressed && !coat.isOpen()) castSpell();
	for (Pickup& pickup : pickups) if (pickup.isActive()) pickup.update(dt);

	const bool anyAlive = std::any_of(enemies.begin(), enemies.end(), [](const Enemy& enemy) { return enemy.isAlive(); });
	if (!anyAlive && messageTimer <= 0.0f) setMessage("THE ROAD IS YOURS // PRESS R TO REPLAY", 999.0f);
}

void Game::setMessage(const std::string& value, float duration) {
	message = value;
	messageTimer = duration;
}

void Game::collectPickups() {
	for (Pickup& pickup : pickups) {
		if (!pickup.isActive()) continue;
		if (flatDistance(player.getPosition(), pickup.getPosition()) < 0.85f) {
			player.addReagent(pickup.getType(), pickup.getQuantity());
			collected += pickup.getQuantity();
			pickup.setActive(false);
			setMessage(std::string(reagentName(pickup.getType())) + " +" + std::to_string(pickup.getQuantity()), 1.4f);
		}
	}
}

void Game::updateEnemies(float dt) {
	const std::vector<BoundingBox> obstacles = obstacleBounds();
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) continue;
		const bool aliveBefore = enemy.isAlive();
		enemy.update(dt, player.getPosition(), obstacles);
		if (aliveBefore && !enemy.isAlive()) {
			const int type = (enemy.getKind() + static_cast<int>(elapsed * 3.0f)) % REAGENT_COUNT;
			pickups.emplace_back(Vector3Add(enemy.getPosition(), Vector3{0.0f, 0.35f, 0.0f}), static_cast<ReagentType>(type));
			continue;
		}
		if (flatDistance(enemy.getPosition(), player.getPosition()) < 1.18f && enemy.canAttack()) {
			player.takeDamage(11.0f + enemy.getKind() * 2.0f);
			enemy.resetAttack();
			setMessage(player.isWardActive() ? "WARD CAUGHT THE BLOW" : "THE HUNTERS HAVE YOU", 0.7f);
		}
	}
}

void Game::applySpellArea(Vector3 center, const SpellStats& stats, float radiusScale) {
	const float radius = std::max(0.75f, stats.splashRadius * radiusScale);
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive() || flatDistance(center, enemy.getPosition()) > radius) continue;
		const Vector3 deathPosition = enemy.getPosition();
		enemy.applyHex(stats);
		if (!enemy.isAlive()) {
			const int type = (enemy.getKind() + static_cast<int>(center.x + 40.0f)) % REAGENT_COUNT;
			pickups.emplace_back(Vector3Add(deathPosition, Vector3{0.0f, 0.35f, 0.0f}), static_cast<ReagentType>(type));
		}
	}
}

void Game::castSpell() {
	CurseVector& curse = player.getSpell();
	const SpellStats stats = curse.compose();
	const BaseVector base = curse.getBase();
	const Vector3 origin = player.getCamera().position;
	const Vector3 direction = player.getLookDirection();
	player.triggerCastFlash();
	player.gainWard(stats.wardGain);

	if (base == BaseVector::Bottle) {
		Vector3 velocity = Vector3Scale(direction, 13.5f);
		velocity.y += 1.2f;
		effects.emplace_back(EffectDelivery::Projectile, Vector3Add(origin, Vector3Scale(direction, 0.65f)), velocity, stats);
		setMessage("BOTTLE HEX CAST", 0.7f);
	} else if (base == BaseVector::Spider) {
		Vector3 location = Vector3Add(player.getPosition(), Vector3Scale(direction, 1.4f));
		location = groundPoint(location);
		for (const BoundingBox& bounds : obstacleBounds()) {
			if (pointInBounds(location, bounds)) location = groundPoint(player.getPosition());
		}
		effects.emplace_back(EffectDelivery::Mine, location, Vector3{}, stats);
		setMessage("WIDOW MINE PLACED", 0.7f);
	} else if (base == BaseVector::Apple) {
		player.heal(stats.healAmount);
		applySpellArea(player.getPosition(), stats);
		if (stats.lingeringDuration > 1.0f) effects.emplace_back(EffectDelivery::Hazard, groundPoint(player.getPosition()), Vector3{}, stats);
		setMessage("BITTER APPLE EATEN", 0.7f);
	} else {
		Enemy* target = nullptr;
		float best = std::numeric_limits<float>::max();
		for (Enemy& enemy : enemies) {
			if (!enemy.isAlive()) continue;
			Vector3 toEnemy = Vector3Subtract(Vector3Add(enemy.getPosition(), Vector3{0, 1.2f, 0}), origin);
			const float distance = Vector3Length(toEnemy);
			if (distance > 22.0f) continue;
			const float aim = Vector3DotProduct(Vector3Normalize(toEnemy), direction);
			if (aim > 0.975f && distance < best) {
				best = distance;
				target = &enemy;
			}
		}
		if (target) {
			const Vector3 hit = target->getPosition();
			target->applyHex(stats);
			if (!target->isAlive()) {
				const int type = target->getKind() % REAGENT_COUNT;
				pickups.emplace_back(Vector3Add(hit, Vector3{0, 0.35f, 0}), static_cast<ReagentType>(type));
			}
			setMessage("POPPET PIN STRIKES", 0.5f);
		} else setMessage("POPPET FINDS NO FLESH", 0.5f);
	}
	player.consumeSelectedReagents();
}

void Game::updateEffects(float dt) {
	std::vector<Effect> spawnedHazards;
	const std::vector<BoundingBox> obstacles = obstacleBounds();
	for (Effect& effect : effects) {
		if (!effect.isActive()) continue;

		if (effect.getDelivery() == EffectDelivery::Projectile && effect.getStats().homingStrength > 0) {
			Enemy* target = nullptr;
			float best = 12.0f + effect.getStats().homingStrength * 4.0f;
			for (Enemy& enemy : enemies) {
				if (!enemy.isAlive()) continue;
				const float distance = Vector3Distance(effect.getPosition(), enemy.getPosition());
				if (distance < best) { best = distance; target = &enemy; }
			}
			if (target) {
				const float speed = Vector3Length(effect.getVelocity());
				const Vector3 desired = Vector3Scale(Vector3Normalize(Vector3Subtract(Vector3Add(target->getPosition(), Vector3{0, 1, 0}), effect.getPosition())), speed);
				effect.setVelocity(Vector3Lerp(effect.getVelocity(), desired, std::min(1.0f, dt * (2.0f + effect.getStats().homingStrength))));
			}
		}

		effect.update(dt);
		bool trigger = false;
		if (effect.getDelivery() == EffectDelivery::Projectile) {
			trigger = effect.getPosition().y <= 0.08f;
			for (const BoundingBox& bounds : obstacles) {
				if (pointInBounds(effect.getPosition(), bounds)) trigger = true;
			}
			for (const Enemy& enemy : enemies) {
				if (enemy.isAlive() && Vector3Distance(effect.getPosition(), Vector3Add(enemy.getPosition(), Vector3{0, 1, 0})) < 0.72f) trigger = true;
			}
		} else if (effect.getDelivery() == EffectDelivery::Mine && effect.getAge() > 0.6f) {
			for (const Enemy& enemy : enemies) {
				if (enemy.isAlive() && flatDistance(effect.getPosition(), enemy.getPosition()) < 2.0f) trigger = true;
			}
		} else if (effect.getDelivery() == EffectDelivery::Hazard) {
			effect.countTick(dt);
			if (effect.getTickTimer() <= 0.0f) {
				SpellStats hazard = effect.getStats();
				hazard.damage = 2.0f + hazard.rotDamagePerSecond * 0.5f;
				applySpellArea(effect.getPosition(), hazard, 1.0f);
				effect.resetTickTimer(0.65f);
			}
		}

		if (trigger) {
			applySpellArea(effect.getPosition(), effect.getStats());
			if (effect.getStats().lingeringDuration > 1.0f) {
				Effect hazard(EffectDelivery::Hazard, groundPoint(effect.getPosition()), Vector3{}, effect.getStats());
				hazard.setLifetime(effect.getStats().lingeringDuration);
				spawnedHazards.push_back(hazard);
			}
			effect.setActive(false);
		}
	}
	effects.erase(std::remove_if(effects.begin(), effects.end(), [](const Effect& effect) { return !effect.isActive(); }), effects.end());
	effects.insert(effects.end(), spawnedHazards.begin(), spawnedHazards.end());
}

void Game::draw() const {
	if (!started) {
		mainMenu.draw();
		return;
	}
	ClearBackground(Color{5, 8, 10, 255});
	BeginMode3D(player.getCamera());
	DrawPlane(Vector3{0, -0.015f, -13}, Vector2{40, 52}, Color{10, 16, 16, 255});
	for (int x = -20; x <= 20; x += 2) DrawLine3D(Vector3{static_cast<float>(x), 0, -39}, Vector3{static_cast<float>(x), 0, 13}, Fade(Color{65, 85, 75, 255}, 0.35f));
	for (int z = -39; z <= 13; z += 2) DrawLine3D(Vector3{-20, 0, static_cast<float>(z)}, Vector3{20, 0, static_cast<float>(z)}, Fade(Color{65, 85, 75, 255}, 0.35f));
	for (const Scenery& object : scenery) object.draw3D();
	for (const Pickup& pickup : pickups) if (pickup.isActive()) pickup.draw3D();
	for (const Effect& effect : effects) if (effect.isActive()) effect.draw3D();
	for (const Enemy& enemy : enemies) if (enemy.isAlive()) enemy.draw3D();
	EndMode3D();

	const int alive = static_cast<int>(std::count_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) { return enemy.isAlive(); }));
	hud.draw(player, alive, collected, message.c_str(), messageTimer, coat.isOpen());
	coat.draw(player);
	if (!player.isAlive()) {
		DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{15, 2, 4, 205});
		const char* death = "THE GALLOWS CLAIM YOU";
		const char* retry = "PRESS R TO RETURN TO THE ROAD";
		DrawText(death, GetScreenWidth() / 2 - MeasureText(death, 38) / 2, GetScreenHeight() / 2 - 34, 38, Color{235, 80, 70, 255});
		DrawText(retry, GetScreenWidth() / 2 - MeasureText(retry, 20) / 2, GetScreenHeight() / 2 + 22, 20, Color{220, 210, 180, 255});
	}
}
