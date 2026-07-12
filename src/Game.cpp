#include <algorithm>
#include <array>
#include <cmath>
#include "Game.h"
#include <raymath.h>

Game::Game(int screenWidth, int screenHeight)
	: screenWidth(screenWidth), screenHeight(screenHeight), rng(std::random_device {}()) {
	mainMenu.setVisible(true);
	loadResources();
	player.setHandsSprite(&handsSprite);
	player.setWardSprite(&wardSprite);
}

Game::~Game() {
	unloadResources();
}

void Game::update() {
	float dt = GetFrameTime();
	if (dt > 0.05f) {
		dt = 0.05f;
	}

	const InputHandler::InputState input = inputHandler.poll();

	switch (state) {
		case State::MainMenu:
			if (mainMenu.update(input)) {
				startRun();
			}
			break;
		case State::Playing:
			updatePlaying(dt, input);
			break;
		case State::Victory:
		case State::Defeat:
			if (input.restartPressed) {
				startRun();
			}
			break;
	}
}

void Game::draw() {
	BeginDrawing();

	switch (state) {
		case State::MainMenu:
			mainMenu.draw(screenWidth, screenHeight);
			break;
		case State::Playing:
		case State::Victory:
		case State::Defeat:
			ClearBackground(Color {8, 10, 16, 255});
			drawWorld();
			player.drawHands2D(screenWidth, screenHeight);
			hud.draw(player, screenWidth, screenHeight, std::max(1, wave), static_cast<int>(enemies.size()), laneCleared);
			coatMenu.draw(player, screenWidth, screenHeight);
			if (state == State::Victory || state == State::Defeat) {
				DrawRectangle(0, 0, screenWidth, screenHeight, Color {0, 0, 0, 170});
				const char* headline = state == State::Victory ? "Salem lane broken" : "The hunters close in";
				const char* prompt = "Press Enter to begin another run";
				DrawText(headline, screenWidth / 2 - 210, screenHeight / 2 - 30, 36, Color {240, 244, 250, 255});
				DrawText(prompt, screenWidth / 2 - 190, screenHeight / 2 + 20, 24, Color {255, 190, 112, 255});
			}
			break;
	}

	EndDrawing();
}

void Game::loadResources() {
	handsSprite.loadFromFile("resources/generated/hands/witch_hands_sheet.png");
	enemySprite.loadFromFile("resources/generated/enemies/witch_hunter_sheet.png");
	fenceSprite.loadFromFile("resources/generated/scenery/fence_segment.png");
	cottageSprite.loadFromFile("resources/generated/buildings/timber_cottage_front.png");
	wardSprite.loadFromFile("resources/generated/sigils/warding_sigil_sheet.png");
}

void Game::unloadResources() {
}

void Game::startRun() {
	state = State::Playing;
	wave = 0;
	restartTimer = 0.0f;
	laneCleared = false;
	effects.clear();
	pickups.clear();
	enemies.clear();
	buildWorld();
	player.reset(Vector3 {0.0f, 0.0f, 24.0f});
	player.setHandsSprite(&handsSprite);
	player.setWardSprite(&wardSprite);

	for (int i = 0; i < ReagentTypeCount; ++i) {
		const float offset = static_cast<float>(i) - 2.5f;
		pickups.emplace_back(Vector3 {offset * 1.2f, 0.0f, 20.0f}, static_cast<ReagentType>(i), 1);
	}

	spawnWave(1);
}

void Game::buildWorld() {
	scenery.clear();
	blockers.clear();

	const std::array<float, 4> laneZ {-24.0f, -10.0f, 4.0f, 18.0f};
	for (float z : laneZ) {
		Scenery leftHouse(Vector3 {-14.5f, 2.5f, z}, Vector3 {7.0f, 5.0f, 6.0f}, SceneryVisual::Cottage, true);
		leftHouse.setSprite(&cottageSprite);
		scenery.push_back(leftHouse);
		blockers.push_back(leftHouse.getBounds());

		Scenery rightHouse(Vector3 {14.5f, 2.5f, z + 2.5f}, Vector3 {7.0f, 5.0f, 6.0f}, SceneryVisual::Cottage, true);
		rightHouse.setSprite(&cottageSprite);
		scenery.push_back(rightHouse);
		blockers.push_back(rightHouse.getBounds());
	}

	for (int i = -5; i <= 5; ++i) {
		Scenery leftFence(Vector3 {-8.0f, 1.1f, static_cast<float>(i) * 6.0f}, Vector3 {4.0f, 2.0f, 0.5f}, SceneryVisual::Fence, true);
		leftFence.setSprite(&fenceSprite);
		scenery.push_back(leftFence);
		blockers.push_back(leftFence.getBounds());

		Scenery rightFence(Vector3 {8.0f, 1.1f, static_cast<float>(i) * 6.0f + 2.8f}, Vector3 {4.0f, 2.0f, 0.5f}, SceneryVisual::Fence, true);
		rightFence.setSprite(&fenceSprite);
		scenery.push_back(rightFence);
		blockers.push_back(rightFence.getBounds());
	}
}

void Game::spawnWave(int waveIndex) {
	wave = waveIndex;
	laneCleared = false;
	restartTimer = 0.0f;

	const int count = 4 + waveIndex * 2;
	for (int i = 0; i < count; ++i) {
		Enemy enemy(randomSpawnPoint());
		enemy.setSprite(&enemySprite);
		enemies.push_back(enemy);
	}
}

void Game::updatePlaying(float dt, const InputHandler::InputState& input) {
	player.update(dt, input, blockers);
	coatMenu.update(player, input);
	handleCasting(input);
	updatePickups(dt);
	updateEffects(dt);
	updateEnemies(dt);
	cleanDeadEnemies();

	if (!player.isAlive()) {
		state = State::Defeat;
		return;
	}

	if (enemies.empty()) {
		laneCleared = true;
		restartTimer += dt;
		if (wave >= 3 && restartTimer > 1.0f) {
			state = State::Victory;
		} else if (wave < 3 && restartTimer > 1.6f) {
			spawnWave(wave + 1);
		}
	}
}

void Game::updateEffects(float dt) {
	for (Effect& effect : effects) {
		effect.tick(dt, player, enemies, blockers);
	}

	effects.erase(
		std::remove_if(effects.begin(), effects.end(), [](const Effect& effect) {
			return effect.hasExpired();
		}),
		effects.end()
	);
}

void Game::updatePickups(float dt) {
	for (Pickup& pickup : pickups) {
		pickup.update(dt);
		if (pickup.tryCollect(player.getPosition(), 1.2f)) {
			player.collect(pickup.getType(), pickup.getAmount());
		}
	}

	pickups.erase(
		std::remove_if(pickups.begin(), pickups.end(), [](const Pickup& pickup) {
			return !pickup.isActive();
		}),
		pickups.end()
	);
}

void Game::updateEnemies(float dt) {
	std::vector<Vector3> positions;
	positions.reserve(enemies.size());
	for (const Enemy& enemy : enemies) {
		if (enemy.isAlive()) {
			positions.push_back(enemy.getPosition());
		}
	}

	for (Enemy& enemy : enemies) {
		enemy.updateAI(dt, player, blockers, positions);
		enemy.tryAttack(dt, player);
	}
}

void Game::handleCasting(const InputHandler::InputState& input) {
	if (!input.castPressed || !player.canCast()) {
		return;
	}

	const SpellPreview spell = player.previewSpell();
	if (!player.consumeForSpell(spell)) {
		return;
	}

	if (spell.base == BaseVectorType::Apple) {
		player.addWard(spell.wardGain);
		effects.push_back(Effect::MakePulse(player.getPosition(), spell));
	} else if (spell.base == BaseVectorType::Spider) {
		Vector3 minePos = Vector3Add(player.getPosition(), Vector3Scale(player.getForward(), 1.4f));
		minePos.y = 0.0f;
		effects.push_back(Effect::MakeMine(minePos, spell));
	} else {
		Vector3 start = player.getCamera().position;
		Vector3 direction = player.getForward();
		effects.push_back(Effect::MakeProjectile(Vector3Add(start, Vector3Scale(direction, 0.8f)), direction, spell));
	}

	player.triggerCastCooldown(spell.cooldown);
}

void Game::cleanDeadEnemies() {
	for (Enemy& enemy : enemies) {
		if (!enemy.shouldDropLoot()) {
			continue;
		}

		std::uniform_int_distribution<int> drops(1, 2);
		std::uniform_int_distribution<int> reagent(0, ReagentTypeCount - 1);
		for (int i = 0; i < drops(rng); ++i) {
			Vector3 dropPos = enemy.getPosition();
			dropPos.x += static_cast<float>(i) * 0.4f - 0.2f;
			pickups.emplace_back(dropPos, static_cast<ReagentType>(reagent(rng)), 1);
		}
		enemy.markLootDropped();
	}

	enemies.erase(
		std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
			return !enemy.isAlive();
		}),
		enemies.end()
	);
}

void Game::drawWorld() const {
	BeginMode3D(player.getCamera());

	DrawPlane(Vector3 {0.0f, 0.0f, 0.0f}, Vector2 {58.0f, 78.0f}, Color {18, 24, 28, 255});
	for (int i = -9; i <= 9; ++i) {
		DrawCube(Vector3 {0.0f, 0.01f, static_cast<float>(i) * 4.0f}, 2.5f, 0.02f, 1.4f, Color {50, 58, 66, 255});
	}

	DrawGrid(24, 2.0f);

	for (const Scenery& piece : scenery) {
		piece.draw3D(player.getCamera());
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

	EndMode3D();
}

Vector3 Game::randomSpawnPoint() {
	std::uniform_real_distribution<float> xDist(-5.5f, 5.5f);
	std::uniform_real_distribution<float> zDist(-30.0f, 8.0f);
	return Vector3 {xDist(rng), 0.0f, zDist(rng)};
}
