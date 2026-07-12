#include "Game.h"

#include <raylib.h>
#include <raymath.h>

#include <algorithm>
#include <cmath>

namespace {
	constexpr Color SKY{8, 11, 20, 255};
	constexpr Color GROUND{18, 23, 30, 255};
	constexpr Color GRID{43, 51, 61, 255};
	constexpr Color LINE{224, 230, 214, 255};
	constexpr Color ACCENT{224, 154, 90, 255};
	constexpr Color WARNING{232, 88, 88, 255};
	constexpr Color WARD{102, 206, 221, 255};

	Vector3 flatNormalize(Vector3 value) {
		value.y = 0.0f;
		const float length = std::sqrt(value.x * value.x + value.z * value.z);
		if (length <= 0.001f) return {0.0f, 0.0f, 0.0f};
		return {value.x / length, 0.0f, value.z / length};
	}

	float flatDistance(Vector3 first, Vector3 second) {
		const float dx = first.x - second.x;
		const float dz = first.z - second.z;
		return std::sqrt(dx * dx + dz * dz);
	}

	const char* enemyName(EnemyKind kind) {
		switch (kind) {
			case EnemyKind::TorchBearer: return "TORCH MOB";
			case EnemyKind::WitchHunter: return "WITCH-HUNTER";
			case EnemyKind::Suppressor: return "SUPPRESSOR";
		}
		return "HOSTILE";
	}
}

Game::Game(int screenWidth, int screenHeight)
	: width(screenWidth), height(screenHeight), input(KEY_NULL) {
	effects.reserve(128);
	input.setMouseCaptured(true);
	spawnLevel();
}

void Game::spawnLevel() {
	scenery.emplace_back(SceneryKind::House, Vector3{-10.0f, 0.0f, -7.2f}, Vector3{5.4f, 3.3f, 4.2f});
	scenery.emplace_back(SceneryKind::House, Vector3{10.0f, 0.0f, -7.2f}, Vector3{5.4f, 3.3f, 4.2f});
	scenery.emplace_back(SceneryKind::House, Vector3{-10.0f, 0.0f, 5.4f}, Vector3{4.2f, 2.8f, 3.2f});
	scenery.emplace_back(SceneryKind::House, Vector3{10.0f, 0.0f, 5.4f}, Vector3{4.2f, 2.8f, 3.2f});
	scenery.emplace_back(SceneryKind::Fence, Vector3{0.0f, 0.0f, -11.0f}, Vector3{22.0f, 1.7f, 0.35f});
	scenery.emplace_back(SceneryKind::Fence, Vector3{-14.0f, 0.0f, 0.0f}, Vector3{0.35f, 1.7f, 18.0f});
	scenery.emplace_back(SceneryKind::Fence, Vector3{14.0f, 0.0f, 0.0f}, Vector3{0.35f, 1.7f, 18.0f});
	scenery.emplace_back(SceneryKind::Tree, Vector3{-13.0f, 0.0f, -3.0f}, Vector3{1.8f, 4.8f, 1.8f}, false);
	scenery.emplace_back(SceneryKind::Tree, Vector3{13.0f, 0.0f, -3.0f}, Vector3{1.8f, 4.8f, 1.8f}, false);
	scenery.emplace_back(SceneryKind::Tree, Vector3{-6.4f, 0.0f, -10.0f}, Vector3{1.5f, 4.1f, 1.5f}, false);
	scenery.emplace_back(SceneryKind::Tree, Vector3{6.4f, 0.0f, -10.0f}, Vector3{1.5f, 4.1f, 1.5f}, false);
	scenery.emplace_back(SceneryKind::Lantern, Vector3{-5.2f, 0.0f, 7.2f}, Vector3{0.7f, 3.0f, 0.7f}, false);
	scenery.emplace_back(SceneryKind::Lantern, Vector3{5.2f, 0.0f, 7.2f}, Vector3{0.7f, 3.0f, 0.7f}, false);
	scenery.emplace_back(SceneryKind::Shrine, Vector3{0.0f, 0.0f, -8.8f}, Vector3{1.3f, 1.8f, 0.8f}, false);

	enemies.emplace_back(EnemyKind::TorchBearer, Vector3{-4.0f, 0.0f, 4.0f});
	enemies.emplace_back(EnemyKind::TorchBearer, Vector3{4.0f, 0.0f, 3.2f});
	enemies.emplace_back(EnemyKind::TorchBearer, Vector3{-5.0f, 0.0f, -1.0f});
	enemies.emplace_back(EnemyKind::WitchHunter, Vector3{3.0f, 0.0f, -1.8f});
	enemies.emplace_back(EnemyKind::WitchHunter, Vector3{-3.0f, 0.0f, -4.5f});
	enemies.emplace_back(EnemyKind::Suppressor, Vector3{0.0f, 0.0f, -6.0f});

	for (int i = 0; i < REAGENT_COUNT; ++i) {
		const float angle = static_cast<float>(i) * PI / 3.0f;
		const Vector3 pickupPosition{std::cos(angle) * 7.4f, 0.48f, 1.8f + std::sin(angle) * 6.0f};
		pickups.emplace_back(ReagentFromIndex(i), pickupPosition);
	}
}

void Game::update(float deltaTime) {
	deltaTime = std::clamp(deltaTime, 0.0f, 0.05f);
	worldTime += deltaTime;
	castTimer = std::max(0.0f, castTimer - deltaTime);
	statusTimer = std::max(0.0f, statusTimer - deltaTime);
	if (statusTimer <= 0.0f) statusMessage.clear();

	const InputHandler::InputState inputState = input.poll();
	if (!player.isAlive()) {
		if (inputState.jumpPressed || inputState.clearSelectionPressed) restartLevel();
		return;
	}
	if (inputState.rummageTogglePressed) rummageToggle = !rummageToggle;
	const bool rummaging = inputState.rummageHeld || rummageToggle;
	if (rummaging != coatMenu.isOpen()) {
		if (rummaging) {
			coatMenu.open();
			input.setMouseCaptured(false);
			setStatus("COAT OPEN: choose a vector and three signs while the road moves", 3.0f);
		} else {
			coatMenu.close();
			input.setMouseCaptured(true);
			setStatus("COAT SHUT: cast the assembled curse", 1.5f);
		}
	}

	InputHandler::InputState playerInput = inputState;
	playerInput.rummageHeld = rummaging;
	player.update(deltaTime, playerInput, collisionBoxes());

	if (coatMenu.isOpen()) {
		coatMenu.update(inputState, player);
	} else {
		if (inputState.numberPressed[0]) coatMenu.selectBase(BaseVector::Bottle);
		if (inputState.numberPressed[1]) coatMenu.selectBase(BaseVector::Apple);
		if (inputState.numberPressed[2]) coatMenu.selectBase(BaseVector::Spider);
		if (inputState.previousBasePressed) coatMenu.cycleBase(-1);
		if (inputState.nextBasePressed) coatMenu.cycleBase(1);
		if ((inputState.castPressed || inputState.castHeld) && castTimer <= 0.0f) castSpell();
	}

	collectPickups();
	updateCombat(deltaTime);
	updateEffects(deltaTime);
}

void Game::updateCombat(float deltaTime) {
	const std::vector<BoundingBox> obstacles = collisionBoxes();
	for (Enemy& enemy : enemies) {
		if (enemy.updateTowardsPlayer(deltaTime, player.getPosition(), obstacles)) {
			player.takeDamage(enemy.attackDamage());
			setStatus(std::string(enemyName(enemy.kind())) + " reached you", 1.0f);
		}
	}

	int defeated = 0;
	for (auto iterator = enemies.begin(); iterator != enemies.end();) {
		if (iterator->isAlive()) {
			++iterator;
			continue;
		}
		const Vector3 dropPosition = iterator->getPosition();
		const int seed = std::abs(static_cast<int>(dropPosition.x * 11.0f + dropPosition.z * 7.0f + worldTime * 3.0f));
		pickups.emplace_back(ReagentFromIndex(seed % REAGENT_COUNT), Vector3{dropPosition.x, 0.48f, dropPosition.z});
		iterator = enemies.erase(iterator);
		++defeated;
	}
	if (defeated > 0) {
		setStatus("A corpse leaves a reagent. The coat remembers.", 2.0f);
		if (enemies.empty() && !levelCompleteMessageShown) {
			levelCompleteMessageShown = true;
			setStatus("ROAD CLEARED // six signs remain yours to test", 5.0f);
		}
	}
}

void Game::updateEffects(float deltaTime) {
	for (std::size_t i = 0; i < effects.size(); ++i) {
		if (!effects[i].isActive()) continue;
		Effect& effect = effects[i];
		const SpellStats stats = effect.spell().stats();

		if (effect.kind() == EffectKind::Projectile && stats.seeking) {
			float nearestDistance = 10.0f;
			Vector3 nearest{};
			bool foundTarget = false;
			for (const Enemy& enemy : enemies) {
				if (!enemy.isAlive()) continue;
				const float distance = flatDistance(effect.getPosition(), enemy.getPosition());
				if (distance < nearestDistance) {
					nearestDistance = distance;
					nearest = enemy.getPosition();
					foundTarget = true;
				}
			}
			if (foundTarget) {
				const Vector3 direction = flatNormalize(Vector3Subtract(nearest, effect.getPosition()));
				const float speed = std::max(8.0f, Vector3Length(effect.velocity()));
				effect.setVelocity(Vector3Scale(direction, speed));
			}
		}

		effect.update(deltaTime);
		if (effect.kind() == EffectKind::Projectile) {
			bool impact = effect.getPosition().x < -15.0f || effect.getPosition().x > 15.0f || effect.getPosition().z < -13.0f || effect.getPosition().z > 13.0f;
			for (const Enemy& enemy : enemies) {
				if (enemy.isAlive() && flatDistance(effect.getPosition(), enemy.getPosition()) <= 0.48f + enemy.collisionRadius()) {
					impact = true;
					break;
				}
			}
			if (impact) {
				const Vector3 impactPosition = effect.getPosition();
				const CurseVector impactSpell = effect.spell();
				effect.setActive(false);
				applyImpact(impactPosition, impactSpell);
			}
		} else if (effect.kind() == EffectKind::Mine && effect.isActive() && effect.armed()) {
			for (const Enemy& enemy : enemies) {
				if (enemy.isAlive() && flatDistance(effect.getPosition(), enemy.getPosition()) <= std::max(1.0f, stats.radius * 0.85f)) {
					const Vector3 impactPosition = effect.getPosition();
					const CurseVector impactSpell = effect.spell();
					effect.setActive(false);
					applyImpact(impactPosition, impactSpell);
					break;
				}
			}
		} else if (effect.kind() == EffectKind::Hazard && effect.pulseReady(0.65f)) {
			SpellStats pulse = stats;
			pulse.damage = stats.damage > 0 ? std::max(1, stats.damage / 4) : 0;
			for (Enemy& enemy : enemies) {
				if (enemy.isAlive() && flatDistance(effect.getPosition(), enemy.getPosition()) <= stats.radius + enemy.collisionRadius()) enemy.applySpell(pulse);
			}
		}
	}

	effects.erase(std::remove_if(effects.begin(), effects.end(), [](const Effect& effect) { return effect.expired(); }), effects.end());
}

void Game::castSpell() {
	CurseVector spell = coatMenu.spell();
	if (player.totalReagents() == 0 && spell.reagentCount() == 0 && spell.base() != BaseVector::Poppet) {
		spell.setBase(BaseVector::Poppet);
		coatMenu.selectBase(BaseVector::Poppet);
	}
	for (int i = 0; i < spell.reagentCount(); ++i) {
		bool alreadyChecked = false;
		for (int j = 0; j < i; ++j) if (spell.reagentAt(j) == spell.reagentAt(i)) alreadyChecked = true;
		if (alreadyChecked) continue;
		if (player.reagentQuantity(spell.reagentAt(i)) < spell.countOf(spell.reagentAt(i))) {
			setStatus("That pocket is empty. Rummage again.", 1.5f);
			return;
		}
	}
	for (int i = 0; i < spell.reagentCount(); ++i) player.consumeReagent(spell.reagentAt(i));

	const SpellStats stats = spell.stats();
	const Vector3 direction = player.forward();
	const Vector3 handPosition = Vector3Add(player.getPosition(), {direction.x * 0.65f, 1.15f, direction.z * 0.65f});
	castTimer = spell.base() == BaseVector::Apple ? 0.45f : (spell.base() == BaseVector::Spider ? 0.35f : 0.16f);

	switch (spell.base()) {
		case BaseVector::Apple: {
			player.heal(12.0f + static_cast<float>(stats.damage) * 0.5f);
			player.addWard(stats.wardGain);
			if (stats.snareDuration > 0.0f) player.setHaste(stats.snareDuration + 1.0f);
			if (stats.poisonPerSecond > 0.0f) player.setRegeneration(2.5f + stats.hazardDuration, stats.poisonPerSecond);
			if (stats.seeking) player.setHaste(1.5f);
			if (stats.fearDuration > 0.0f) {
				for (Enemy& enemy : enemies) {
					if (enemy.isAlive() && flatDistance(player.getPosition(), enemy.getPosition()) <= stats.radius + 1.0f) enemy.applySpell(stats);
				}
			}
			effects.emplace_back(EffectKind::Burst, player.getPosition(), Vector3{}, spell, 0.32f);
			setStatus("ATE " + spell.name() + " // " + spell.outcomeText(), 2.0f);
			break;
		}
		case BaseVector::Spider: {
			const Vector3 minePosition{player.getPosition().x + direction.x * 2.0f, 0.0f, player.getPosition().z + direction.z * 2.0f};
			effects.emplace_back(EffectKind::Mine, minePosition, Vector3{}, spell, 15.0f);
			setStatus("PLACED " + spell.name() + " // trap armed", 2.0f);
			break;
		}
		case BaseVector::Bottle:
		case BaseVector::Poppet: {
			const float speed = spell.base() == BaseVector::Poppet ? 12.0f : 14.0f;
			effects.emplace_back(EffectKind::Projectile, handPosition, Vector3Scale(direction, speed), spell, 3.0f);
			setStatus("THREW " + spell.name() + " // " + spell.outcomeText(), 2.0f);
			break;
		}
	}
	if (spell.reagentCount() > 0) coatMenu.clearSelection();
}

void Game::restartLevel() {
	scenery.clear();
	enemies.clear();
	pickups.clear();
	effects.clear();
	player = Player();
	coatMenu = CoatMenu();
	rummageToggle = false;
	levelCompleteMessageShown = false;
	input.setMouseCaptured(true);
	setStatus("THE ROAD REFORMS // gather the six signs", 3.0f);
	spawnLevel();
}

void Game::applyImpact(Vector3 impactPosition, const CurseVector& spell) {
	const SpellStats stats = spell.stats();
	if (stats.wardGain > 0.0f) player.addWard(stats.wardGain);
	const float radius = std::max(0.45f, stats.radius);
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) continue;
		if (flatDistance(impactPosition, enemy.getPosition()) <= radius + enemy.collisionRadius()) enemy.applySpell(stats);
	}
	effects.emplace_back(EffectKind::Burst, impactPosition, Vector3{}, spell, 0.25f);
	if (stats.hazardDuration > 0.0f) effects.emplace_back(EffectKind::Hazard, impactPosition, Vector3{}, spell, stats.hazardDuration);
	setStatus("HEX IMPACT // " + spell.outcomeText(), 1.2f);
}

void Game::collectPickups() {
	for (Pickup& pickup : pickups) {
		if (!pickup.isActive()) continue;
		pickup.update(GetFrameTime());
		if (pickup.canCollect(player.getPosition(), player.collisionRadius())) {
			const ReagentType type = pickup.type();
			player.addReagent(type);
			pickup.setActive(false);
			setStatus("TAKEN: " + std::string(GetReagentDefinition(type).name) + " // pocket " + GetReagentDefinition(type).shortName, 1.7f);
		}
	}
}

void Game::setStatus(const std::string& message, float duration) {
	statusMessage = message;
	statusTimer = duration;
}

std::vector<BoundingBox> Game::collisionBoxes() const {
	std::vector<BoundingBox> boxes;
	boxes.reserve(scenery.size());
	for (const Scenery& object : scenery) if (object.isSolid()) boxes.push_back(object.bounds());
	return boxes;
}

void Game::draw() const {
	BeginDrawing();
	ClearBackground(SKY);
	drawWorld();
	drawWorldMarkers();

	hud.draw(player, coatMenu.spell(), coatMenu.isOpen(), castTimer <= 0.0f, statusMessage);
	if (coatMenu.isOpen()) coatMenu.draw(player, width, height);
	player.drawHands(width, height, coatMenu.isOpen(), castTimer > 0.0f);
	if (!player.isAlive()) {
		DrawRectangle(0, 0, width, height, ColorAlpha({5, 5, 10, 255}, 0.7f));
		DrawText("THE ROAD CLAIMED YOU", width / 2 - MeasureText("THE ROAD CLAIMED YOU", 32) / 2, height / 2 - 28, 32, WARNING);
		DrawText("press SPACE or BACKSPACE to rise again", width / 2 - MeasureText("press SPACE or BACKSPACE to rise again", 18) / 2, height / 2 + 20, 18, LINE);
	}
	EndDrawing();
}

void Game::drawWorld() const {
	BeginMode3D(player.camera());
	DrawPlane({0.0f, -0.015f, 0.0f}, {29.0f, 23.0f}, GROUND);
	DrawGrid(24, 1.0f);
	DrawLine3D({-14.4f, 0.02f, -11.4f}, {14.4f, 0.02f, -11.4f}, ACCENT);
	DrawLine3D({-14.4f, 0.02f, 11.4f}, {14.4f, 0.02f, 11.4f}, ACCENT);
	DrawLine3D({-14.4f, 0.02f, -11.4f}, {-14.4f, 0.02f, 11.4f}, ACCENT);
	DrawLine3D({14.4f, 0.02f, -11.4f}, {14.4f, 0.02f, 11.4f}, ACCENT);

	for (const Scenery& object : scenery) object.draw();
	for (const Pickup& pickup : pickups) if (pickup.isActive()) pickup.draw();
	for (const Effect& effect : effects) if (effect.isActive()) effect.draw();
	for (const Enemy& enemy : enemies) if (enemy.isAlive()) enemy.draw();
	DrawCircle3D({player.getPosition().x, 0.025f, player.getPosition().z}, 0.55f, {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(WARD, 0.22f));
	EndMode3D();
}

void Game::drawWorldMarkers() const {
	for (const Enemy& enemy : enemies) {
		if (!enemy.isAlive()) continue;
		const Vector2 screen = GetWorldToScreen({enemy.getPosition().x, enemy.getPosition().y + 2.35f, enemy.getPosition().z}, player.camera());
		if (screen.x < 0.0f || screen.x > static_cast<float>(width) || screen.y < 0.0f || screen.y > static_cast<float>(height)) continue;
		const int barWidth = 74;
		DrawRectangle(static_cast<int>(screen.x - barWidth * 0.5f), static_cast<int>(screen.y), barWidth, 7, ColorAlpha(WARNING, 0.2f));
		DrawRectangle(static_cast<int>(screen.x - barWidth * 0.5f), static_cast<int>(screen.y), static_cast<int>(barWidth * enemy.health() / enemy.maxHealth()), 7, enemy.accentColor());
		DrawRectangleLines(static_cast<int>(screen.x - barWidth * 0.5f), static_cast<int>(screen.y), barWidth, 7, LINE);
		if (enemy.recentlyHit() || flatDistance(player.getPosition(), enemy.getPosition()) < 4.5f) {
			const char* label = enemyName(enemy.kind());
			DrawText(label, static_cast<int>(screen.x - MeasureText(label, 10) * 0.5f), static_cast<int>(screen.y - 15.0f), 10, ColorAlpha(LINE, 0.85f));
		}
	}
}
