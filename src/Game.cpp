#include "Game.h"

#include <raymath.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace {
Color SpellColor(const SpellStats& stats) {
	if (stats.wardGain > 0.0f) return {90, 229, 241, 255};
	if (stats.poisonDps > 0.0f) return {180, 255, 70, 255};
	if (stats.fearDuration > 0.0f) return {255, 108, 220, 255};
	if (stats.snareStrength > 0.0f) return {119, 255, 111, 255};
	return {255, 82, 99, 255};
}
}

Game::Game()
	: input_(KEY_NULL), player_({0.0f, 0.0f, 18.0f}) {
	buildArena();
}

void Game::startNewRun() {
	input_.captureMouse(true);
	resetRun();
	state_ = State::Playing;
}

void Game::update(float dt) {
	const InputHandler::InputState input = input_.poll();
	if (state_ == State::MainMenu) {
		if (mainMenu_.update(input)) {
			startNewRun();
		}
		return;
	}

	if (state_ == State::GameOver || state_ == State::Victory) {
		if (input.confirmPressed) {
			startNewRun();
		}
		return;
	}

	if (!input.mouseCaptured) {
		hud_.update(dt);
		return;
	}
	updatePlaying(input, dt);
}

void Game::draw() const {
	BeginDrawing();
	if (state_ == State::MainMenu) {
		mainMenu_.draw();
		EndDrawing();
		return;
	}

	ClearBackground({3, 7, 12, 255});
	drawWorld();
	drawWorldLabels();
	coatMenu_.draw(player_, spell_);
	hud_.draw(player_, spell_, coatMenu_.isActive(), wave_, livingEnemyCount(),
		static_cast<int>(std::count_if(pickups_.begin(), pickups_.end(), [](const Pickup& pickup) { return pickup.isActive(); })));
	hud_.drawHands(player_, coatMenu_.isActive());

	if (!input_.isMouseCaptured() && state_ == State::Playing) {
		const int width = GetScreenWidth();
		DrawRectangle(0, 0, width, 54, {3, 7, 11, 230});
		const char* prompt = "CURSOR FREE // WORLD HELD // PRESS ESC TO RETURN";
		DrawText(prompt, width / 2 - MeasureText(prompt, 18) / 2, 18, 18, {90, 229, 241, 255});
	}
	if (state_ == State::GameOver || state_ == State::Victory) {
		hud_.drawTerminalOverlay(state_ == State::Victory, kills_);
	}
	EndDrawing();
}

bool Game::shouldClose() const {
	return closeRequested_ || WindowShouldClose();
}

void Game::resetRun() {
	player_.reset({0.0f, 0.0f, 18.0f});
	spell_ = CurseVector {};
	coatMenu_.reset();
	enemies_.clear();
	pickups_.clear();
	projectiles_.clear();
	traps_.clear();
	fields_.clear();
	bursts_.clear();
	traces_.clear();
	wave_ = 1;
	kills_ = 0;
	nextWaveTimer_ = 0.0f;
	wardWasActive_ = false;
	spawnInitialPickups();
	spawnWave(wave_);
	hud_.setNotice("THE POPPET IS ALWAYS READY // FIND THE SIX MARKED REAGENTS", 4.0f);
}

void Game::buildArena() {
	scenery_.clear();
	const auto box = [](float x, float y, float z, float sx, float sy, float sz) {
		return BoundingBox {{x - sx / 2.0f, y, z - sz / 2.0f}, {x + sx / 2.0f, y + sy, z + sz / 2.0f}};
	};

	scenery_.emplace_back(box(0.0f, 0.0f, -29.7f, 60.0f, 2.2f, 0.6f), SceneryStyle::Fence);
	scenery_.emplace_back(box(0.0f, 0.0f, 29.7f, 60.0f, 2.2f, 0.6f), SceneryStyle::Fence);
	scenery_.emplace_back(box(-29.7f, 0.0f, 0.0f, 0.6f, 2.2f, 60.0f), SceneryStyle::Fence);
	scenery_.emplace_back(box(29.7f, 0.0f, 0.0f, 0.6f, 2.2f, 60.0f), SceneryStyle::Fence);

	scenery_.emplace_back(box(-19.0f, 0.0f, -12.0f, 8.0f, 4.2f, 7.0f), SceneryStyle::Building);
	scenery_.emplace_back(box(19.0f, 0.0f, -10.0f, 8.0f, 4.8f, 8.0f), SceneryStyle::Building);
	scenery_.emplace_back(box(-20.5f, 0.0f, 12.0f, 7.0f, 3.8f, 6.0f), SceneryStyle::Building);
	scenery_.emplace_back(box(21.0f, 0.0f, 12.5f, 7.0f, 4.0f, 6.5f), SceneryStyle::Building);
	scenery_.emplace_back(box(-5.5f, 0.0f, 4.0f, 2.2f, 1.4f, 2.2f), SceneryStyle::Crate);
	scenery_.emplace_back(box(6.0f, 0.0f, 1.0f, 2.5f, 1.1f, 1.8f), SceneryStyle::Crate);
	scenery_.emplace_back(box(-9.0f, 0.0f, -11.0f, 2.4f, 4.5f, 0.5f), SceneryStyle::Gallows);
	scenery_.emplace_back(box(11.0f, 0.0f, -17.0f, 3.0f, 5.5f, 3.0f), SceneryStyle::Tree, false);
	scenery_.emplace_back(box(-13.0f, 0.0f, 16.0f, 3.0f, 5.0f, 3.0f), SceneryStyle::Tree, false);
	scenery_.emplace_back(box(10.0f, 0.0f, 9.0f, 0.8f, 1.2f, 0.35f), SceneryStyle::Grave);
	scenery_.emplace_back(box(12.0f, 0.0f, 8.0f, 0.8f, 1.0f, 0.35f), SceneryStyle::Grave);
	scenery_.emplace_back(box(14.0f, 0.0f, 9.5f, 0.8f, 1.35f, 0.35f), SceneryStyle::Grave);
}

void Game::spawnWave(int wave) {
	static const std::array<Vector3, 12> spawnPoints {{
		{0.0f, 0.0f, -20.0f}, {-8.0f, 0.0f, -16.0f}, {8.0f, 0.0f, -13.0f},
		{-14.0f, 0.0f, -2.0f}, {14.0f, 0.0f, -1.0f}, {-10.0f, 0.0f, 8.0f},
		{16.0f, 0.0f, 5.0f}, {-3.0f, 0.0f, -8.0f}, {4.0f, 0.0f, -22.0f},
		{-22.0f, 0.0f, 2.0f}, {22.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 2.0f}
	}};
	const int count = 3 + wave * 2;
	for (int i = 0; i < count; ++i) {
		const int pointIndex = (i + (wave - 1) * 3) % static_cast<int>(spawnPoints.size());
		const EnemyArchetype archetype = wave >= 2 && i % 3 == 2 ? EnemyArchetype::WitchHunter : EnemyArchetype::TorchBearer;
		const ReagentType drop = static_cast<ReagentType>((i + wave * 2) % static_cast<int>(REAGENT_TYPE_COUNT));
		enemies_.emplace_back(spawnPoints[pointIndex], archetype, wave - 1, drop);
	}
	hud_.setNotice(TextFormat("BREACH %i // %i HUNTERS CROSS THE LINE", wave, count), 2.6f);
	audio_.play(GameSound::NewWave);
}

void Game::spawnInitialPickups() {
	static const std::array<Vector3, 12> positions {{
		{-5.0f, 0.0f, 14.0f}, {-3.0f, 0.0f, 13.5f}, {-1.0f, 0.0f, 14.0f},
		{1.0f, 0.0f, 14.0f}, {3.0f, 0.0f, 13.5f}, {5.0f, 0.0f, 14.0f},
		{-5.0f, 0.0f, 10.0f}, {-3.0f, 0.0f, 9.5f}, {-1.0f, 0.0f, 10.0f},
		{1.0f, 0.0f, 10.0f}, {3.0f, 0.0f, 9.5f}, {5.0f, 0.0f, 10.0f}
	}};
	for (std::size_t i = 0; i < positions.size(); ++i) {
		pickups_.emplace_back(positions[i], static_cast<ReagentType>(i % REAGENT_TYPE_COUNT));
	}
}

void Game::updatePlaying(const InputHandler::InputState& input, float dt) {
	hud_.update(dt);
	coatMenu_.update(input, dt, player_, spell_);
	const std::vector<BoundingBox> obstacles = collisionGeometry();
	player_.updatePlayer(input, dt, obstacles, coatMenu_.isActive());
	if (player_.isWardActive() && !wardWasActive_) {
		audio_.play(GameSound::WardRaise);
	}
	wardWasActive_ = player_.isWardActive();
	updatePickups(dt);

	if (input.castPressed && !coatMenu_.isActive()) {
		castSpell();
	}
	updateEffects(dt);
	updateEnemies(dt, coatMenu_.isActive());
	collectEnemyDrops();

	if (!player_.isAlive()) {
		coatMenu_.setActive(false);
		state_ = State::GameOver;
		return;
	}

	if (livingEnemyCount() == 0) {
		nextWaveTimer_ += dt;
		if (wave_ < 3 && nextWaveTimer_ >= 2.5f) {
			++wave_;
			nextWaveTimer_ = 0.0f;
			spawnWave(wave_);
		} else if (wave_ >= 3 && nextWaveTimer_ >= 2.0f) {
			state_ = State::Victory;
			coatMenu_.setActive(false);
		}
	} else {
		nextWaveTimer_ = 0.0f;
	}
}

void Game::updateEnemies(float dt, bool vulnerable) {
	const std::vector<BoundingBox> obstacles = collisionGeometry();
	for (Enemy& enemy : enemies_) {
		if (!enemy.isAlive()) {
			continue;
		}
		const float damage = enemy.updateCombat(dt, player_.position(), obstacles);
		if (damage > 0.0f) {
			const float healthBefore = player_.health();
			const float wardBefore = player_.ward();
			player_.takeIncomingDamage(damage, vulnerable);
			if (player_.health() < healthBefore) {
				audio_.play(GameSound::Hurt);
			} else if (player_.ward() < wardBefore) {
				audio_.play(GameSound::WardImpact);
			}
			if (enemy.archetype() == EnemyArchetype::WitchHunter) {
				traces_.emplace_back(Vector3 {enemy.position().x, enemy.position().y + 1.35f, enemy.position().z},
					player_.camera().position, Color {255, 184, 72, 255});
			}
		}
	}
}

void Game::updateEffects(float dt) {
	const std::vector<BoundingBox> obstacles = collisionGeometry();
	for (ProjectileEffect& projectile : projectiles_) {
		Vector3 target {};
		const bool hasTarget = findHomingTarget(projectile.position(), target);
		projectile.updateProjectile(dt, target, hasTarget);
		const Vector3 segment = Vector3Subtract(projectile.position(), projectile.previousPosition());
		const float segmentLength = Vector3Length(segment);
		bool impact = false;
		Vector3 impactPoint = projectile.position();
		float nearest = segmentLength + 1.0f;
		if (segmentLength > 0.0001f) {
			const Ray ray {projectile.previousPosition(), Vector3Scale(segment, 1.0f / segmentLength)};
			if (projectile.position().y <= 0.03f && projectile.previousPosition().y > 0.03f) {
				const float fraction = (projectile.previousPosition().y - 0.03f) /
					(projectile.previousPosition().y - projectile.position().y);
				nearest = segmentLength * fraction;
				impactPoint = Vector3Add(projectile.previousPosition(), Vector3Scale(ray.direction, nearest));
				impact = true;
			}
			for (const BoundingBox obstacle : obstacles) {
				const RayCollision collision = GetRayCollisionBox(ray, obstacle);
				if (collision.hit && collision.distance <= segmentLength && collision.distance < nearest) {
					nearest = collision.distance;
					impactPoint = collision.point;
					impact = true;
				}
			}
			for (const Enemy& enemy : enemies_) {
				if (!enemy.isAlive()) continue;
				const RayCollision collision = GetRayCollisionBox(ray, enemy.bounds());
				if (collision.hit && collision.distance <= segmentLength && collision.distance < nearest) {
					nearest = collision.distance;
					impactPoint = collision.point;
					impact = true;
				}
			}
		}
		if (impact) {
			detonate(impactPoint, projectile.payload());
			projectile.setActive(false);
		}
	}

	for (TrapEffect& trap : traps_) {
		trap.update(dt);
		if (trap.expired()) continue;
		for (const Enemy& enemy : enemies_) {
			if (enemy.isAlive() && trap.contains({enemy.position().x, trap.position().y, enemy.position().z}, enemy.targetRadius())) {
				detonate(trap.position(), trap.payload());
				trap.setActive(false);
				break;
			}
		}
	}

	for (HexFieldEffect& field : fields_) {
		field.updateField(dt, player_.position());
		while (!field.expired() && field.takePulse()) {
			applyFieldPulse(field);
		}
	}
	for (BurstEffect& burst : bursts_) burst.update(dt);
	for (TraceEffect& trace : traces_) trace.update(dt);

	projectiles_.erase(std::remove_if(projectiles_.begin(), projectiles_.end(), [](const ProjectileEffect& effect) { return effect.expired(); }), projectiles_.end());
	traps_.erase(std::remove_if(traps_.begin(), traps_.end(), [](const TrapEffect& effect) { return effect.expired(); }), traps_.end());
	fields_.erase(std::remove_if(fields_.begin(), fields_.end(), [](const HexFieldEffect& effect) { return effect.expired(); }), fields_.end());
	bursts_.erase(std::remove_if(bursts_.begin(), bursts_.end(), [](const BurstEffect& effect) { return effect.expired(); }), bursts_.end());
	traces_.erase(std::remove_if(traces_.begin(), traces_.end(), [](const TraceEffect& effect) { return effect.expired(); }), traces_.end());
}

void Game::updatePickups(float dt) {
	for (Pickup& pickup : pickups_) {
		if (!pickup.isActive()) continue;
		pickup.update(dt);
		if (pickup.collectIfNear(player_.position(), 1.05f)) {
			player_.collect(pickup.type(), pickup.quantity());
			audio_.play(GameSound::Pickup);
			const auto& definition = GetReagentDefinition(pickup.type());
			hud_.setNotice(std::string(definition.name) + " TUCKED INTO THE COAT");
		}
	}
}

void Game::castSpell() {
	if (!player_.canCast()) {
		return;
	}
	if (!spell_.canConsume(player_.inventory())) {
		hud_.setNotice("THE SELECTED LOOPS ARE EMPTY // HOLD R");
		coatMenu_.clampSelectionToInventory(player_, spell_);
		return;
	}

	const SpellStats stats = spell_.stats();
	const Vector3 direction = player_.viewDirection();
	const Vector3 start = Vector3Add(player_.camera().position, Vector3Scale(direction, 0.55f));
	switch (spell_.base()) {
	case BaseVectorType::Poppet: {
		audio_.play(GameSound::PoppetCast);
		const Ray ray {player_.camera().position, direction};
		float nearest = 45.0f;
		Vector3 end = Vector3Add(ray.position, Vector3Scale(ray.direction, nearest));
		for (const BoundingBox obstacle : collisionGeometry()) {
			const RayCollision collision = GetRayCollisionBox(ray, obstacle);
			if (collision.hit && collision.distance < nearest) {
				nearest = collision.distance;
				end = collision.point;
			}
		}
		Enemy* victim = nullptr;
		for (Enemy& enemy : enemies_) {
			if (!enemy.isAlive()) continue;
			const RayCollision hit = GetRayCollisionBox(ray, enemy.bounds());
			if (hit.hit && hit.distance < nearest) {
				nearest = hit.distance;
				end = hit.point;
				victim = &enemy;
			}
		}
		if (victim != nullptr && victim->applyHex(stats) > 0.0f) {
			player_.registerHit();
			audio_.play(GameSound::HexImpact);
		}
		traces_.emplace_back(start, end, Color {255, 198, 116, 255});
		break;
	}
	case BaseVectorType::Bottle: {
		audio_.play(GameSound::BottleCast);
		Vector3 velocity = Vector3Scale(direction, stats.projectileSpeed);
		velocity.y += 0.8f;
		projectiles_.emplace_back(start, velocity, stats);
		break;
	}
	case BaseVectorType::Apple: {
		audio_.play(GameSound::AppleCast);
		player_.heal(stats.heal);
		player_.addWard(stats.wardGain);
		SpellStats auraStats = stats;
		auraStats.wardGain = 0.0f;
		auraStats.lingering = false;
		fields_.emplace_back(Vector3 {player_.position().x, 0.03f, player_.position().z}, auraStats, stats.duration, true);
		applyFieldPulse(fields_.back());
		break;
	}
	case BaseVectorType::Spider: {
		audio_.play(GameSound::SpiderCast);
		const Vector3 forward = player_.flatForward();
		Vector3 placement = Vector3Add(player_.position(), Vector3Scale(forward, 1.45f));
		for (const BoundingBox obstacle : collisionGeometry()) {
			if (CheckCollisionBoxSphere(obstacle, {placement.x, 0.2f, placement.z}, 0.24f)) {
				placement = Vector3Add(player_.position(), Vector3Scale(forward, 0.55f));
				break;
			}
		}
		traps_.emplace_back(Vector3 {placement.x, 0.02f, placement.z}, stats);
		break;
	}
	}

	const bool consumed = player_.consumeFor(spell_);
	(void)consumed;
	player_.beginCast(stats.cooldown);
	coatMenu_.clampSelectionToInventory(player_, spell_);
}

void Game::detonate(Vector3 position, const SpellStats& stats, bool createLingeringField) {
	audio_.play(GameSound::HexImpact);
	int affected = 0;
	float dealt = 0.0f;
	for (Enemy& enemy : enemies_) {
		if (!enemy.isAlive()) continue;
		Vector3 target = enemy.position();
		target.y = position.y;
		if (Vector3Distance(position, target) <= stats.radius + enemy.targetRadius()) {
			dealt += enemy.applyHex(stats);
			++affected;
		}
	}
	if (affected > 0) {
		player_.registerHit();
		if (stats.wardGain > 0.0f) {
			player_.addWard(stats.wardGain);
		}
	}
	if (createLingeringField && stats.lingering) {
		SpellStats fieldStats = stats;
		fieldStats.wardGain = 0.0f;
		fieldStats.lingering = false;
		fields_.emplace_back(Vector3 {position.x, 0.03f, position.z}, fieldStats, stats.duration, false);
	}
	bursts_.emplace_back(position, std::max(0.55f, stats.radius), SpellColor(stats));
	(void)dealt;
}

void Game::applyFieldPulse(const HexFieldEffect& field) {
	SpellStats tick = field.payload();
	tick.damage *= 0.22f;
	tick.wardGain = 0.0f;
	tick.lingering = false;
	bool hit = false;
	for (Enemy& enemy : enemies_) {
		if (!enemy.isAlive()) continue;
		Vector3 target = enemy.position();
		target.y = field.position().y;
		if (field.contains(target, enemy.targetRadius())) {
			enemy.applyHex(tick);
			hit = true;
		}
	}
	if (hit) {
		player_.registerHit();
	}
}

void Game::collectEnemyDrops() {
	for (Enemy& enemy : enemies_) {
		if (enemy.claimDrop()) {
			pickups_.emplace_back(Vector3 {enemy.position().x, 0.0f, enemy.position().z}, enemy.dropType());
			++kills_;
			audio_.play(GameSound::EnemyDeath);
			hud_.registerKill();
			hud_.setNotice(std::string(GetReagentDefinition(enemy.dropType()).name) + " SHAKES LOOSE");
		}
	}
}

void Game::drawWorld() const {
	BeginMode3D(player_.camera());
	drawArenaGround();
	for (const Scenery& scenery : scenery_) scenery.draw(player_.camera());
	for (const Pickup& pickup : pickups_) pickup.draw(player_.camera());
	for (const Enemy& enemy : enemies_) enemy.draw(player_.camera());
	for (const ProjectileEffect& effect : projectiles_) effect.draw(player_.camera());
	for (const TrapEffect& effect : traps_) effect.draw(player_.camera());
	for (const HexFieldEffect& effect : fields_) effect.draw(player_.camera());
	for (const BurstEffect& effect : bursts_) effect.draw(player_.camera());
	for (const TraceEffect& effect : traces_) effect.draw(player_.camera());
	EndMode3D();
}

void Game::drawArenaGround() const {
	DrawPlane({0.0f, -0.025f, 0.0f}, {60.0f, 60.0f}, {7, 12, 16, 255});
	for (int i = -30; i <= 30; i += 2) {
		const Color line = i % 10 == 0 ? Color {33, 76, 83, 255} : Color {18, 42, 49, 255};
		DrawLine3D({static_cast<float>(i), 0.0f, -30.0f}, {static_cast<float>(i), 0.0f, 30.0f}, line);
		DrawLine3D({-30.0f, 0.0f, static_cast<float>(i)}, {30.0f, 0.0f, static_cast<float>(i)}, line);
	}
	DrawSphere({14.0f, 20.0f, -27.0f}, 2.2f, {183, 221, 214, 255});
	DrawSphereWires({14.0f, 20.0f, -27.0f}, 2.3f, 7, 12, {95, 224, 226, 255});
}

void Game::drawWorldLabels() const {
	for (const Enemy& enemy : enemies_) enemy.drawBillboardHud(player_.camera());
	for (const Pickup& pickup : pickups_) pickup.drawBillboardHud(player_.camera());
}

std::vector<BoundingBox> Game::collisionGeometry() const {
	std::vector<BoundingBox> result;
	result.reserve(scenery_.size());
	for (const Scenery& scenery : scenery_) {
		if (scenery.isActive() && scenery.isCollisionEnabled()) {
			result.push_back(scenery.bounds());
		}
	}
	return result;
}

int Game::livingEnemyCount() const {
	return static_cast<int>(std::count_if(enemies_.begin(), enemies_.end(), [](const Enemy& enemy) {
		return enemy.isAlive();
	}));
}

bool Game::findHomingTarget(Vector3 position, Vector3& target) const {
	float nearest = std::numeric_limits<float>::max();
	bool found = false;
	for (const Enemy& enemy : enemies_) {
		if (!enemy.isAlive()) continue;
		const Vector3 candidate {enemy.position().x, enemy.position().y + 0.9f, enemy.position().z};
		const float distance = Vector3Distance(position, candidate);
		if (distance < nearest && distance <= 18.0f) {
			nearest = distance;
			target = candidate;
			found = true;
		}
	}
	return found;
}
