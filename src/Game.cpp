#include "Game.h"

#include <algorithm>
#include <array>
#include <cmath>

#include <raylib.h>
#include <raymath.h>

Game::Game(int initialScreenWidth, int initialScreenHeight)
	: screenWidth(initialScreenWidth), screenHeight(initialScreenHeight) {
	inputHandler.setMouseCaptured(false);
	createWorld();
}

void Game::update() {
	float deltaTime = GetFrameTime();
	deltaTime = std::min(deltaTime, 0.05f);
	const InputHandler::InputState input = inputHandler.poll();

	if (!gameStarted) {
		if (mainMenu.update(input)) {
			startGame();
		}
		return;
	}
	if (!player.isAlive()) {
		if (input.confirmPressed || input.castPressed || input.rummageHeld) {
			resetGame();
		}
		return;
	}

	elapsedTime += deltaTime;
	announcementTime = std::max(0.0f, announcementTime - deltaTime);
	const CoatMenuInputResult coatInput = coatMenu.update(input, player.getInventory(), GetScreenWidth(), GetScreenHeight());
	if (coatMenu.isOpen() != coatWasOpen) {
		inputHandler.setMouseCaptured(!coatMenu.isOpen());
		coatWasOpen = coatMenu.isOpen();
	}

	player.update(input, deltaTime, getWorldColliders(), coatMenu.isOpen());
	updatePickups(deltaTime);
	updateEnemies(deltaTime);
	updateEffects(deltaTime);

	if ((input.castPressed && !coatInput.consumedMouseClick) || coatInput.castRequested) {
		castSelectedSpell();
	}

}

void Game::draw() {
	BeginDrawing();
	ClearBackground({5, 9, 17, 255});

	if (!gameStarted) {
		mainMenu.draw(GetScreenWidth(), GetScreenHeight());
		EndDrawing();
		return;
	}

	BeginMode3D(player.getCamera());
	DrawPlane({0.0f, -0.01f, 0.0f}, {48.0f, 48.0f}, {12, 23, 31, 255});
	DrawGrid(48, 1.0f);
	for (const Scenery& object : scenery) {
		object.draw3D();
	}
	for (const Pickup& pickup : pickups) {
		pickup.draw3D();
	}
	for (const Enemy& enemy : enemies) {
		enemy.draw3D();
	}
	for (const Effect& effect : effects) {
		effect.draw3D();
	}
	EndMode3D();

	player.drawHands(elapsedTime, coatMenu.getSpell().getStats(), coatMenu.isOpen());
	hud.draw(player, coatMenu.getSpell(), coatMenu.isOpen(), announcement.c_str(), announcementTime);
	coatMenu.draw(GetScreenWidth(), GetScreenHeight());

	if (!player.isAlive()) {
		const char* death = "YOU WERE HUNTED  —  [ENTER / R / LMB] RESTITCH THE HUNT";
		DrawText(death, (GetScreenWidth() - MeasureText(death, 34)) / 2, GetScreenHeight() / 2 - 17, 34, {255, 106, 106, 255});
	}

	EndDrawing();
}

void Game::startGame() {
	gameStarted = true;
	inputHandler.setMouseCaptured(true);
	announce("Poppet is ready.  Collect glowing reagents and hold R to stitch a hex.", 4.0f);
}

void Game::resetGame() {
	player = Player {};
	coatMenu = CoatMenu {};
	scenery.clear();
	enemies.clear();
	pickups.clear();
	effects.clear();
	elapsedTime = 0.0f;
	announcementTime = 0.0f;
	coatWasOpen = false;
	inputHandler.setMouseCaptured(true);
	createWorld();
	announce("The thread is restitched.  Four breaths of grace remain.", 3.0f);
}

void Game::createWorld() {
	scenery.emplace_back(SceneryType::Building, Vector3 {-10.5f, 2.6f, -10.5f}, Vector3 {5.6f, 5.2f, 5.4f});
	scenery.emplace_back(SceneryType::Building, Vector3 {10.0f, 2.8f, -12.0f}, Vector3 {5.2f, 5.6f, 5.8f});
	scenery.emplace_back(SceneryType::Building, Vector3 {-15.0f, 2.2f, 4.0f}, Vector3 {4.8f, 4.4f, 5.4f});
	scenery.emplace_back(SceneryType::Building, Vector3 {15.0f, 2.3f, 4.0f}, Vector3 {4.8f, 4.6f, 5.4f});
	scenery.emplace_back(SceneryType::StandingStone, Vector3 {0.0f, 1.0f, -7.0f}, Vector3 {1.3f, 2.0f, 1.0f});
	scenery.emplace_back(SceneryType::Fence, Vector3 {-5.5f, 0.65f, 1.0f}, Vector3 {5.0f, 1.3f, 0.16f});
	scenery.emplace_back(SceneryType::Fence, Vector3 {6.5f, 0.65f, -2.0f}, Vector3 {4.5f, 1.3f, 0.16f});
	scenery.emplace_back(SceneryType::Lantern, Vector3 {-3.5f, 1.1f, -2.5f}, Vector3 {0.3f, 2.2f, 0.3f}, false);
	scenery.emplace_back(SceneryType::Lantern, Vector3 {4.8f, 1.1f, -6.4f}, Vector3 {0.3f, 2.2f, 0.3f}, false);
	scenery.emplace_back(SceneryType::Lantern, Vector3 {-7.0f, 1.1f, -11.0f}, Vector3 {0.3f, 2.2f, 0.3f}, false);

	enemies.emplace_back(EnemyType::TorchBearer, Vector3 {-4.0f, 0.0f, -1.0f});
	enemies.emplace_back(EnemyType::TorchBearer, Vector3 {4.8f, 0.0f, -5.6f});
	enemies.emplace_back(EnemyType::WitchHunter, Vector3 {-5.5f, 0.0f, -8.0f});
	enemies.emplace_back(EnemyType::Hound, Vector3 {7.5f, 0.0f, -10.0f});
	enemies.emplace_back(EnemyType::TorchBearer, Vector3 {-10.0f, 0.0f, -14.0f});
	enemies.emplace_back(EnemyType::WitchHunter, Vector3 {11.0f, 0.0f, -12.0f});

	pickups.emplace_back(Vector3 {1.8f, 0.0f, 5.2f}, ReagentType::GraveSalt);
	pickups.emplace_back(Vector3 {-2.0f, 0.0f, 5.5f}, ReagentType::SaintAsh);
	pickups.emplace_back(Vector3 {5.5f, 0.0f, 1.0f}, ReagentType::BlackWax);
	pickups.emplace_back(Vector3 {-5.0f, 0.0f, -1.5f}, ReagentType::WidowNettle);
	pickups.emplace_back(Vector3 {2.0f, 0.0f, -4.0f}, ReagentType::MothDust);
	pickups.emplace_back(Vector3 {-1.5f, 0.0f, -5.4f}, ReagentType::ChurchGlass);
}

void Game::updatePickups(float deltaTime) {
	for (Pickup& pickup : pickups) {
		pickup.update(deltaTime);
		if (pickup.tryCollect(player.getPosition())) {
			player.addReagent(pickup.getType());
			announce(std::string("Collected ") + getReagentDefinition(pickup.getType()).name + ".", 1.5f);
		}
	}
	pickups.erase(std::remove_if(pickups.begin(), pickups.end(), [](const Pickup& pickup) {
		return pickup.isCollected();
	}), pickups.end());
}

void Game::updateEnemies(float deltaTime) {
	for (Enemy& enemy : enemies) {
		enemy.update(deltaTime, player.getPosition());
		const float incomingDamage = enemy.consumePendingDamage();
		if (incomingDamage > 0.0f && elapsedTime >= 4.0f) {
			player.takeDamage(incomingDamage);
			announce(player.isWardActive() ? "Ward catches the hunter's blow." : "A hunter's blow cuts through.", 0.8f);
		}
		if (enemy.justDied()) {
			const ReagentType drop = reagentFromIndex(static_cast<std::size_t>(GetRandomValue(0, static_cast<int>(ReagentCount) - 1)));
			pickups.emplace_back(enemy.getPosition(), drop);
			SpellStats deathVisual {};
			deathVisual.color = {255, 106, 106, 255};
			effects.push_back(Effect::createBurst(enemy.getPosition(), deathVisual, 0.55f));
			enemy.markDeathHandled();
			announce(std::string(enemy.getName()) + " falls.  A reagent spills free.", 1.6f);
		}
	}
}

void Game::updateEffects(float deltaTime) {
	std::vector<Effect> spawnedEffects;
	for (Effect& effect : effects) {
		effect.update(deltaTime);
		if (!effect.isActive()) {
			continue;
		}

		if (effect.getKind() == EffectKind::Projectile) {
			if (effect.getPosition().y <= 0.05f) {
				effect.setPosition({effect.getPosition().x, 0.08f, effect.getPosition().z});
				effect.detonate();
			} else {
				if (effect.getSpell().seekingStrength > 0.0f) {
					if (Enemy* target = findSeekingTarget(effect.getPosition(), 14.0f)) {
						Vector3 targetDirection = Vector3Subtract(target->getPosition(), effect.getPosition());
						targetDirection.y += 0.7f;
						targetDirection = Vector3Normalize(targetDirection);
						const float speed = Vector3Length(effect.getVelocity());
						const Vector3 redirected = Vector3Scale(targetDirection, speed);
						effect.setVelocity(Vector3Lerp(effect.getVelocity(), redirected, std::min(0.12f, effect.getSpell().seekingStrength * 0.08f)));
					}
				}
				for (const Enemy& enemy : enemies) {
					if (enemy.isAlive() && CheckCollisionSpheres(effect.getPosition(), 0.24f, enemy.getPosition(), enemy.getCollisionRadius())) {
						effect.setPosition(enemy.getPosition());
						effect.detonate();
						break;
					}
				}
			}
		}

		if (effect.getKind() == EffectKind::Mine && effect.getAge() > 0.4f) {
			for (const Enemy& enemy : enemies) {
				if (enemy.isAlive() && effect.isInRadius(enemy.getPosition(), enemy.getCollisionRadius())) {
					effect.detonate();
					break;
				}
			}
		}

		if ((effect.getKind() == EffectKind::Burst || effect.getKind() == EffectKind::Hazard) && effect.consumePulse()) {
			applySpellToEnemies(effect);
			if (effect.getKind() == EffectKind::Burst && effect.getSpell().hazardDuration > 0.0f) {
				spawnedEffects.push_back(Effect::createHazard(effect.getPosition(), effect.getSpell()));
			}
		}
	}

	effects.insert(effects.end(), spawnedEffects.begin(), spawnedEffects.end());
	effects.erase(std::remove_if(effects.begin(), effects.end(), [](const Effect& effect) {
		return !effect.isActive();
	}), effects.end());
}

void Game::castSelectedSpell() {
	if (!player.canCast()) {
		return;
	}

	const CurseVector& selectedSpell = coatMenu.getSpell();
	if (!player.consumeReagents(selectedSpell)) {
		announce("That reagent stack is empty.  Rummage again.", 1.5f);
		return;
	}

	const SpellStats spell = selectedSpell.getStats();
	const Vector3 forward = player.getForward();
	player.beginCast(spell.castCooldown);

	switch (selectedSpell.getBase()) {
	case CurseVectorType::Bottle: {
		Vector3 velocity = Vector3Scale(forward, spell.projectileSpeed);
		velocity.y += 1.7f;
		effects.push_back(Effect::createProjectile(Vector3Add(player.getEyePosition(), Vector3Scale(forward, 0.6f)), velocity, spell));
		announce("Hex bottle loosed.", 0.75f);
		break;
	}
	case CurseVectorType::Apple:
		player.heal(spell.heal);
		player.addWard(spell.wardGain);
		effects.push_back(Effect::createBurst(player.getPosition(), spell, spell.splashRadius));
		announce("You bite the bitter apple: health and ward surge.", 1.1f);
		break;
	case CurseVectorType::Spider: {
		Vector3 minePosition = Vector3Add(player.getPosition(), Vector3Scale(forward, 0.9f));
		minePosition.y = 0.0f;
		effects.push_back(Effect::createMine(minePosition, spell));
		announce("Widow spider placed.  It waits beneath the hunter.", 1.0f);
		break;
	}
	case CurseVectorType::Poppet: {
		const Vector3 start = player.getEyePosition();
		if (Enemy* target = findSeekingTarget(start, 24.0f)) {
			target->applySpell(spell);
			if (spell.wardGain > 0.0f) {
				player.addWard(spell.wardGain);
			}
			effects.push_back(Effect::createTracer(start, Vector3Add(target->getPosition(), Vector3 {0.0f, 1.0f, 0.0f}), spell.color));
			SpellStats impactVisual {};
			impactVisual.color = spell.color;
			effects.push_back(Effect::createBurst(target->getPosition(), impactVisual, 0.36f));
			announce("Poppet needle finds a pulse.", 0.7f);
		} else {
			const Vector3 end = Vector3Add(start, Vector3Scale(forward, 18.0f));
			effects.push_back(Effect::createTracer(start, end, spell.color));
			announce("The poppet finds no living thread.", 0.7f);
		}
		break;
	}
	}
}

void Game::applySpellToEnemies(const Effect& effect) {
	const float damageScale = effect.getKind() == EffectKind::Hazard ? 0.24f : 1.0f;
	bool struckEnemy = false;
	for (Enemy& enemy : enemies) {
		if (enemy.isAlive() && effect.isInRadius(enemy.getPosition(), enemy.getCollisionRadius())) {
			enemy.applySpell(effect.getSpell(), damageScale);
			struckEnemy = true;
		}
	}
	if (struckEnemy && effect.getSpell().wardGain > 0.0f) {
		player.addWard(effect.getSpell().wardGain * damageScale);
	}
}

Enemy* Game::findSeekingTarget(Vector3 origin, float range) {
	const Vector3 forward = player.getForward();
	Enemy* bestTarget = nullptr;
	float bestScore = -1000.0f;
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) {
			continue;
		}
		Vector3 toEnemy = Vector3Subtract(enemy.getPosition(), origin);
		toEnemy.y += 0.8f;
		const float distance = Vector3Length(toEnemy);
		if (distance > range || distance <= 0.01f) {
			continue;
		}
		const float facing = Vector3DotProduct(forward, Vector3Normalize(toEnemy));
		if (facing < 0.55f) {
			continue;
		}
		const float score = facing * 4.0f - distance * 0.06f;
		if (score > bestScore) {
			bestScore = score;
			bestTarget = &enemy;
		}
	}
	return bestTarget;
}

void Game::announce(const std::string& text, float duration) {
	announcement = text;
	announcementTime = duration;
}

std::vector<BoundingBox> Game::getWorldColliders() const {
	std::vector<BoundingBox> colliders;
	for (const Scenery& object : scenery) {
		if (object.isCollisionEnabled()) {
			colliders.push_back(object.getBounds());
		}
	}
	return colliders;
}
