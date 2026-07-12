#include "Game.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <utility>

#include <raymath.h>

namespace {
constexpr Color SkyColor {5, 9, 17, 255};
constexpr Color GroundColor {8, 18, 22, 255};
constexpr Color GridColor {23, 57, 61, 255};
constexpr Color SalemLine {128, 216, 194, 255};
constexpr int MaximumPlacedSpiders = 4;

Vector3 onGround(Vector3 position) {
	position.y = 0.0f;
	return position;
}
}

Game::Game()
	: inputHandler(KEY_NULL) {
	inputHandler.setMouseCaptured(false);
	handSprite.load(assetPath("resources/generated/hands/witch_hands_sheet.png"), 8, 3);
	enemySprite.load(assetPath("resources/generated/enemies/witch_hunter_sheet.png"), 8, 3);
	buildWorld();
	resetRun();
}

void Game::update() {
	float deltaTime = GetFrameTime();
	deltaTime = Clamp(deltaTime, 0.0f, 0.05f);
	const InputHandler::InputState input = inputHandler.poll();

	if (mainMenu.isOpen()) {
		if (mainMenu.update(input)) {
			inputHandler.setMouseCaptured(true);
			showNotification("THE HUNTERS HAVE FOUND YOU.", 2.5f);
		}
		return;
	}

	if (!player.isAlive()) {
		if (input.startPressed) {
			resetRun();
			inputHandler.setMouseCaptured(true);
		}
		return;
	}

	notificationTime = std::max(0.0f, notificationTime - deltaTime);
	coatMenu.update(input, inventory, GetScreenWidth(), GetScreenHeight());
	if (coatMenu.isOpen() != coatWasOpen) {
		coatWasOpen = coatMenu.isOpen();
		inputHandler.setMouseCaptured(!coatWasOpen);
	}
	player.update(deltaTime, input, solidBounds, coatMenu.isOpen());

	if (coatMenu.isOpen()) {
		if (coatMenu.takeCastRequest()) {
			castCurrentCurse();
		}
	} else if (input.castPressed) {
		castCurrentCurse();
	}

	for (Effect& effect : effects) {
		effect.update(deltaTime, solidBounds, enemies);
	}
	effects.erase(std::remove_if(effects.begin(), effects.end(), [](const Effect& effect) {
		return !effect.isActive();
	}), effects.end());

	for (Enemy& enemy : enemies) {
		enemy.update(deltaTime, player, solidBounds);
	}
	collectEnemyDrops();
	for (Pickup& pickup : pickups) {
		pickup.update(deltaTime);
	}
	collectPickups();
	pickups.erase(std::remove_if(pickups.begin(), pickups.end(), [](const Pickup& pickup) {
		return !pickup.isActive();
	}), pickups.end());
}

void Game::draw() {
	BeginDrawing();
	ClearBackground(SkyColor);

	if (mainMenu.isOpen()) {
		mainMenu.draw(GetScreenWidth(), GetScreenHeight());
		EndDrawing();
		return;
	}

	drawWorld();
	hud.draw(player, coatMenu, inventory, &handSprite, livingEnemyCount(), notification, notificationTime);
	coatMenu.draw(inventory, GetScreenWidth(), GetScreenHeight());

	if (!player.isAlive()) {
		const int width = GetScreenWidth();
		const int height = GetScreenHeight();
		DrawRectangle(0, 0, width, height, Color {20, 0, 6, 175});
		const char* title = "THE HUNTERS HAVE SILENCED YOU";
		DrawText(title, width / 2 - MeasureText(title, 28) / 2, height / 2 - 32, 28, Color {255, 137, 117, 255});
		const char* restart = "ENTER / SPACE / CLICK TO RETURN";
		DrawText(restart, width / 2 - MeasureText(restart, 18) / 2, height / 2 + 14, 18, RAYWHITE);
	}

	EndDrawing();
}

void Game::resetRun() {
	player = Player {};
	coatMenu = CoatMenu {};
	inventory.clear();
	enemies.clear();
	pickups.clear();
	effects.clear();
	notification.clear();
	notificationTime = 0.0f;
	coatWasOpen = false;

	const std::array<ReagentType, kReagentTypeCount> types {
		ReagentType::GraveSalt,
		ReagentType::ChurchGlass,
		ReagentType::WidowNettle,
		ReagentType::PlagueHoney,
		ReagentType::MothDust,
		ReagentType::BlackWax
	};

	pickups.emplace_back(Vector3 {-1.9f, 0.0f, 9.1f}, ReagentType::GraveSalt);
	pickups.emplace_back(Vector3 {1.8f, 0.0f, 8.6f}, ReagentType::WidowNettle);
	pickups.emplace_back(Vector3 {-3.4f, 0.0f, 5.2f}, ReagentType::ChurchGlass);
	pickups.emplace_back(Vector3 {3.7f, 0.0f, 4.0f}, ReagentType::PlagueHoney);
	pickups.emplace_back(Vector3 {-4.0f, 0.0f, -1.0f}, ReagentType::MothDust);
	pickups.emplace_back(Vector3 {4.2f, 0.0f, -2.4f}, ReagentType::BlackWax);

	const std::array<Vector3, 9> spawnPoints {
		Vector3 {0.0f, 0.0f, 5.0f}, Vector3 {-4.0f, 0.0f, 2.0f}, Vector3 {4.2f, 0.0f, 1.3f},
		Vector3 {-6.2f, 0.0f, -2.0f}, Vector3 {5.8f, 0.0f, -3.7f}, Vector3 {0.0f, 0.0f, -3.0f},
		Vector3 {-7.4f, 0.0f, -7.5f}, Vector3 {6.9f, 0.0f, -8.7f}, Vector3 {0.0f, 0.0f, -11.0f}
	};
	for (std::size_t index = 0; index < spawnPoints.size(); ++index) {
		const EnemyArchetype archetype = index % 3 == 0 ? EnemyArchetype::WitchHunter : EnemyArchetype::TorchBearer;
		enemies.emplace_back(spawnPoints[index], archetype, types[index % types.size()]);
	}
}

void Game::buildWorld() {
	scenery.clear();
	solidBounds.clear();

	scenery.emplace_back(Vector3 {0.0f, 0.0f, -13.0f}, Vector3 {7.0f, 5.6f, 3.6f}, true,
		Scenery::Style::Chapel, Color {244, 173, 112, 255});
	scenery.emplace_back(Vector3 {-10.5f, 0.0f, -4.0f}, Vector3 {4.6f, 3.4f, 4.0f}, true,
		Scenery::Style::Box, SalemLine);
	scenery.emplace_back(Vector3 {10.2f, 0.0f, -5.0f}, Vector3 {4.8f, 4.0f, 4.4f}, true,
		Scenery::Style::Box, SalemLine);
	scenery.emplace_back(Vector3 {-9.2f, 0.0f, 7.0f}, Vector3 {2.0f, 2.2f, 8.0f}, true,
		Scenery::Style::Box, Color {111, 190, 177, 255});
	scenery.emplace_back(Vector3 {9.3f, 0.0f, 7.0f}, Vector3 {2.0f, 2.2f, 8.0f}, true,
		Scenery::Style::Box, Color {111, 190, 177, 255});
	scenery.emplace_back(Vector3 {-1.5f, 0.0f, -3.0f}, Vector3 {1.3f, 2.4f, 1.3f}, true,
		Scenery::Style::Box, Color {125, 240, 204, 255});
	scenery.emplace_back(Vector3 {3.1f, 0.0f, -0.8f}, Vector3 {1.4f, 2.8f, 1.4f}, true,
		Scenery::Style::Box, Color {125, 240, 204, 255});

	for (const Scenery& item : scenery) {
		if (item.isSolid()) {
			solidBounds.push_back(item.getBounds());
		}
	}
}

void Game::castCurrentCurse() {
	const CurseVector& vector = coatMenu.getCurseVector();
	if (!vector.canConsume(inventory)) {
		showNotification("MISSING REAGENT STACKS", 1.2f);
		return;
	}

	const SpellStats spell = vector.stats();
	const CurseVectorType base = vector.base();
	Vector3 trapPosition {};
	if (base == CurseVectorType::Spider) {
		trapPosition = onGround(Vector3Add(player.getPosition(), Vector3Scale(player.getForward(), 1.3f)));
		if (activeTrapCount() >= MaximumPlacedSpiders) {
			showNotification("ONLY FOUR SPIDER HEXES MAY NEST AT ONCE", 1.4f);
			return;
		}
		if (!isValidTrapPosition(trapPosition)) {
			showNotification("NO SPIDER NEST INSIDE A WALL", 1.2f);
			return;
		}
	}

	float cooldown = 0.20f;
	switch (base) {
	case CurseVectorType::Bottle:
		cooldown = 0.65f;
		break;
	case CurseVectorType::Apple:
		cooldown = 1.50f;
		break;
	case CurseVectorType::Spider:
		cooldown = 0.75f;
		break;
	case CurseVectorType::Poppet:
		break;
	}
	if (!player.beginCast(cooldown)) {
		return;
	}
	if (!coatMenu.consumeForCast(inventory)) {
		return;
	}

	const Vector3 forward = player.getForward();
	if (base == CurseVectorType::Bottle) {
		effects.push_back(Effect::makeThrownBottle(player.getCastOrigin(), forward, spell));
		showNotification("BOTTLE CURSE THROWN", 0.9f);
	} else if (base == CurseVectorType::Spider) {
		effects.push_back(Effect::makePlacedSpider(trapPosition, spell));
		showNotification("SPIDER HEX PLACED", 0.9f);
	} else if (base == CurseVectorType::Apple) {
		const SelfHexStats self = selfHexStats(spell);
		player.applySelfHex(self);
		Vector3 auraPosition = onGround(player.getPosition());
		auraPosition.y = 0.72f;
		effects.push_back(Effect::makeSelfHexAura(auraPosition, spell));
		showNotification(TextFormat("BITTER COMMUNION: WARD +%.0f", self.wardGain), 1.1f);
	} else {
		effects.push_back(Effect::makePoppetNeedle(player.getCastOrigin(), forward, spell));
		showNotification("POPPET NEEDLE", 0.65f);
	}
}

void Game::collectPickups() {
	for (Pickup& pickup : pickups) {
		if (!pickup.collectIfNearby(player.getPosition(), 0.78f)) {
			continue;
		}

		inventory.add(pickup.getType(), pickup.getAmount());
		showNotification(TextFormat("TAKEN: %s +%d", Reagent::definition(pickup.getType()).name.data(), pickup.getAmount()), 1.4f);
	}
}

void Game::collectEnemyDrops() {
	for (Enemy& enemy : enemies) {
		ReagentType dropType = ReagentType::GraveSalt;
		if (!enemy.takeDrop(dropType)) {
			continue;
		}

		pickups.emplace_back(onGround(enemy.getPosition()), dropType);
		showNotification("A HUNTER DROPS A REAGENT", 1.4f);
	}
}

void Game::drawWorld() const {
	BeginMode3D(player.getCamera());
	DrawPlane(Vector3 {0.0f, -0.01f, 0.0f}, Vector2 {58.0f, 58.0f}, GroundColor);
	for (int line = -14; line <= 14; ++line) {
		const float axis = static_cast<float>(line) * 2.0f;
		DrawLine3D(Vector3 {axis, 0.0f, -28.0f}, Vector3 {axis, 0.0f, 28.0f}, GridColor);
		DrawLine3D(Vector3 {-28.0f, 0.0f, axis}, Vector3 {28.0f, 0.0f, axis}, GridColor);
	}

	DrawSphereWires(Vector3 {-11.0f, 12.0f, -23.0f}, 1.7f, 10, 10, Color {210, 228, 233, 200});
	DrawLine3D(Vector3 {-12.8f, 0.0f, -19.0f}, Vector3 {-12.8f, 6.0f, -19.0f}, Color {235, 178, 105, 255});
	DrawLine3D(Vector3 {-14.0f, 4.8f, -19.0f}, Vector3 {-11.6f, 4.8f, -19.0f}, Color {235, 178, 105, 255});

	for (const Scenery& item : scenery) {
		item.draw(player.getCamera());
	}
	for (const Pickup& pickup : pickups) {
		pickup.draw(player.getCamera());
	}
	for (const Effect& effect : effects) {
		effect.draw(player.getCamera());
	}
	const Texture2D* enemyTexture = enemySprite.loaded() ? &enemySprite.texture() : nullptr;
	for (const Enemy& enemy : enemies) {
		enemy.draw(player.getCamera(), enemyTexture);
	}
	if (player.isWardActive()) {
		const Vector3 wardCentre {player.getPosition().x, 0.85f, player.getPosition().z};
		DrawSphereWires(wardCentre, 1.05f, 10, 10, Color {94, 223, 238, 180});
		DrawCircle3D(Vector3 {wardCentre.x, 0.04f, wardCentre.z}, 1.05f,
			Vector3 {1.0f, 0.0f, 0.0f}, 90.0f, Color {94, 223, 238, 180});
	}
	EndMode3D();
}

void Game::showNotification(std::string text, float duration) {
	notification = std::move(text);
	notificationTime = duration;
}

bool Game::isValidTrapPosition(Vector3 position) const {
	const Vector3 centre {position.x, 0.22f, position.z};
	for (const BoundingBox& bounds : solidBounds) {
		if (CheckCollisionBoxSphere(bounds, centre, 0.34f)) {
			return false;
		}
	}

	return true;
}

int Game::activeTrapCount() const {
	return static_cast<int>(std::count_if(effects.begin(), effects.end(), [](const Effect& effect) {
		return effect.isActive() && effect.getKind() == EffectKind::PlacedSpider;
	}));
}

int Game::livingEnemyCount() const {
	return static_cast<int>(std::count_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
		return enemy.isAlive();
	}));
}

std::string Game::assetPath(const char* relativePath) const {
	if (FileExists(relativePath)) {
		return relativePath;
	}

	const char* applicationPath = GetApplicationDirectory();
	std::string applicationDirectory = applicationPath != nullptr ? applicationPath : "";
	if (!applicationDirectory.empty()) {
		if (applicationDirectory.back() != '/' && applicationDirectory.back() != '\\') {
			applicationDirectory += '/';
		}
		const std::string besideExecutable = applicationDirectory + relativePath;
		if (FileExists(besideExecutable.c_str())) {
			return besideExecutable;
		}
	}

	return std::string("../") + relativePath;
}
