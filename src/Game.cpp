#include "Game.h"
#include <algorithm>
#include <cmath>
#include <raymath.h>

namespace {
constexpr float EYE_HEIGHT = 1.72f;
constexpr float PLAYER_RADIUS = 0.35f;
constexpr float ARENA_LIMIT = 18.0f;
constexpr int FINAL_WAVE = 3;

Vector3 ForwardFromYawPitch(float yaw, float pitch) {
	return Vector3{
		std::sin(yaw) * std::cos(pitch),
		std::sin(pitch),
		std::cos(yaw) * std::cos(pitch)
	};
}

Vector3 FlatForward(float yaw) {
	return Vector3{std::sin(yaw), 0.0f, std::cos(yaw)};
}

Vector3 FlatRight(float yaw) {
	return Vector3{std::cos(yaw), 0.0f, -std::sin(yaw)};
}

float DistanceXZ(Vector3 a, Vector3 b) {
	float dx = a.x - b.x;
	float dz = a.z - b.z;
	return std::sqrt(dx * dx + dz * dz);
}

Vector3 NormalizeSafe(Vector3 value) {
	float len = Vector3Length(value);
	if (len <= 0.0001f) return Vector3{};
	return Vector3Scale(value, 1.0f / len);
}

Color EnemyColor(EnemyKind kind) {
	switch (kind) {
	case EnemyKind::TorchMob:
		return Color{255, 122, 66, 255};
	case EnemyKind::Musketeer:
		return Color{255, 220, 128, 255};
	case EnemyKind::WardEngineer:
		return Color{110, 210, 255, 255};
	}

	return WHITE;
}

void DrawBillboardGlyph(const Camera3D& camera, Vector3 position, float width, float height, Color color, bool cross) {
	Vector3 toCamera = Vector3Subtract(camera.position, position);
	toCamera.y = 0.0f;
	toCamera = NormalizeSafe(toCamera);
	if (Vector3Length(toCamera) <= 0.0f) toCamera = Vector3{0.0f, 0.0f, 1.0f};

	Vector3 right = Vector3{toCamera.z, 0.0f, -toCamera.x};
	Vector3 up = Vector3{0.0f, 1.0f, 0.0f};
	Vector3 center = Vector3Add(position, Vector3{0.0f, height * 0.5f, 0.0f});
	Vector3 halfRight = Vector3Scale(right, width * 0.5f);
	Vector3 halfUp = Vector3Scale(up, height * 0.5f);
	Vector3 a = Vector3Subtract(Vector3Subtract(center, halfRight), halfUp);
	Vector3 b = Vector3Add(Vector3Subtract(center, halfUp), halfRight);
	Vector3 c = Vector3Add(Vector3Add(center, halfRight), halfUp);
	Vector3 d = Vector3Add(Vector3Subtract(center, halfRight), halfUp);

	DrawTriangle3D(a, b, c, Color{color.r, color.g, color.b, 44});
	DrawTriangle3D(a, c, d, Color{color.r, color.g, color.b, 44});
	DrawLine3D(a, b, color);
	DrawLine3D(b, c, color);
	DrawLine3D(c, d, color);
	DrawLine3D(d, a, color);
	if (cross) {
		DrawLine3D(a, c, color);
		DrawLine3D(b, d, color);
	}
}

void DrawArenaFence() {
	Color fence = Color{92, 110, 92, 255};
	for (int i = -18; i <= 18; i += 3) {
		DrawLine3D(Vector3{static_cast<float>(i), 0.0f, -ARENA_LIMIT}, Vector3{static_cast<float>(i), 1.2f, -ARENA_LIMIT}, fence);
		DrawLine3D(Vector3{static_cast<float>(i), 0.0f, ARENA_LIMIT}, Vector3{static_cast<float>(i), 1.2f, ARENA_LIMIT}, fence);
		DrawLine3D(Vector3{-ARENA_LIMIT, 0.0f, static_cast<float>(i)}, Vector3{-ARENA_LIMIT, 1.2f, static_cast<float>(i)}, fence);
		DrawLine3D(Vector3{ARENA_LIMIT, 0.0f, static_cast<float>(i)}, Vector3{ARENA_LIMIT, 1.2f, static_cast<float>(i)}, fence);
	}
	DrawCubeWires(Vector3{0.0f, 0.6f, -ARENA_LIMIT}, ARENA_LIMIT * 2.0f, 1.2f, 0.1f, fence);
	DrawCubeWires(Vector3{0.0f, 0.6f, ARENA_LIMIT}, ARENA_LIMIT * 2.0f, 1.2f, 0.1f, fence);
	DrawCubeWires(Vector3{-ARENA_LIMIT, 0.6f, 0.0f}, 0.1f, 1.2f, ARENA_LIMIT * 2.0f, fence);
	DrawCubeWires(Vector3{ARENA_LIMIT, 0.6f, 0.0f}, 0.1f, 1.2f, ARENA_LIMIT * 2.0f, fence);
}

bool PointInsideObstacleXZ(Vector3 point, const Obstacle& obstacle, float padding) {
	return point.x > obstacle.center.x - obstacle.size.x * 0.5f - padding &&
		point.x < obstacle.center.x + obstacle.size.x * 0.5f + padding &&
		point.z > obstacle.center.z - obstacle.size.z * 0.5f - padding &&
		point.z < obstacle.center.z + obstacle.size.z * 0.5f + padding;
}
}

Game::Game(int screenWidth, int screenHeight)
	: screenWidth(screenWidth),
	  screenHeight(screenHeight),
	  inputHandler(KEY_NULL) {
	reset();
}

void Game::reset() {
	player = Player{};
	enemies.clear();
	pickups.clear();
	effects.clear();
	obstacles.clear();
	flowState = GameFlowState::Playing;
	wave = 0;
	waveMessageTimer = 3.0f;
	yaw = 0.0f;
	pitch = 0.0f;
	spawnArena();
	spawnWave(0);
}

void Game::spawnArena() {
	player.setPosition(Vector3{0.0f, EYE_HEIGHT, -9.0f});
	player.giveReagent(ReagentType::GraveSalt, 2);
	player.giveReagent(ReagentType::WidowNettle, 2);
	player.giveReagent(ReagentType::MothDust, 1);
	player.giveReagent(ReagentType::SaintAsh, 1);

	pickups = {
		{ReagentType::BlackWax, Vector3{-3.0f, 0.2f, -1.0f}, 0.0f},
		{ReagentType::BellFilings, Vector3{3.5f, 0.2f, -0.5f}, 1.4f},
		{ReagentType::SaintAsh, Vector3{0.0f, 0.2f, 3.0f}, 2.8f},
	};

	obstacles = {
		{Vector3{-7.0f, 1.0f, -2.0f}, Vector3{2.8f, 2.0f, 2.8f}},
		{Vector3{7.0f, 1.0f, 1.0f}, Vector3{3.2f, 2.0f, 2.2f}},
		{Vector3{0.0f, 1.1f, 8.5f}, Vector3{1.5f, 2.2f, 1.5f}},
	};
}

void Game::spawnWave(int waveIndex) {
	wave = waveIndex;
	waveMessageTimer = 3.0f;

	switch (waveIndex) {
	case 0:
		enemies.push_back({EnemyKind::TorchMob, Vector3{-6.0f, 0.0f, 5.0f}, {}, 0.45f, 34.0f, 34.0f});
		enemies.push_back({EnemyKind::TorchMob, Vector3{5.0f, 0.0f, 7.0f}, {}, 0.45f, 34.0f, 34.0f});
		break;
	case 1:
		enemies.push_back({EnemyKind::TorchMob, Vector3{-10.0f, 0.0f, 11.0f}, {}, 0.45f, 38.0f, 38.0f});
		enemies.push_back({EnemyKind::Musketeer, Vector3{0.0f, 0.0f, 13.5f}, {}, 0.5f, 48.0f, 48.0f});
		enemies.push_back({EnemyKind::TorchMob, Vector3{10.0f, 0.0f, 11.0f}, {}, 0.45f, 38.0f, 38.0f});
		pickups.push_back({ReagentType::MothDust, Vector3{-2.0f, 0.2f, 5.0f}, static_cast<float>(GetTime())});
		break;
	case 2:
		enemies.push_back({EnemyKind::Musketeer, Vector3{-12.0f, 0.0f, 13.5f}, {}, 0.5f, 52.0f, 52.0f});
		enemies.push_back({EnemyKind::WardEngineer, Vector3{0.0f, 0.0f, 15.0f}, {}, 0.55f, 76.0f, 76.0f});
		enemies.push_back({EnemyKind::Musketeer, Vector3{12.0f, 0.0f, 13.5f}, {}, 0.5f, 52.0f, 52.0f});
		enemies.push_back({EnemyKind::TorchMob, Vector3{0.0f, 0.0f, 5.5f}, {}, 0.45f, 42.0f, 42.0f});
		pickups.push_back({ReagentType::GraveSalt, Vector3{2.0f, 0.2f, 5.0f}, static_cast<float>(GetTime())});
		break;
	default:
		flowState = GameFlowState::Victory;
		break;
	}
}

void Game::advanceWaveIfCleared() {
	if (flowState != GameFlowState::Playing) return;

	for (const Enemy& enemy : enemies) {
		if (enemy.alive) return;
	}

	if (wave + 1 >= FINAL_WAVE) {
		flowState = GameFlowState::Victory;
		waveMessageTimer = 0.0f;
		return;
	}

	spawnWave(wave + 1);
}

void Game::update(float dt) {
	if (dt > 0.05f) dt = 0.05f;
	InputHandler::InputState input = inputHandler.poll();

	if (IsKeyPressed(KEY_R) && flowState != GameFlowState::Playing) {
		reset();
		return;
	}

	if (waveMessageTimer > 0.0f) waveMessageTimer -= dt;

	coatMenu.open = IsKeyDown(KEY_E) && player.health > 0.0f && flowState == GameFlowState::Playing;
	coatMenu.update(player);

	if (flowState == GameFlowState::Playing) {
		updatePlayer(dt, input);
		updateEnemies(dt);
		if (player.health <= 0.0f) flowState = GameFlowState::Defeat;
	}
	updateEffects(dt);
	updatePickups(dt);
	player.updateWard(dt);
	advanceWaveIfCleared();

	if (!coatMenu.open && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player.health > 0.0f && flowState == GameFlowState::Playing) {
		castCurrentSpell();
	}
}

void Game::updatePlayer(float dt, const InputHandler::InputState& input) {
	const float lookSensitivity = 0.0025f;
	yaw -= input.lookInput.x * lookSensitivity;
	pitch -= input.lookInput.y * lookSensitivity;
	pitch = std::clamp(pitch, -1.25f, 1.25f);

	Vector3 forward = FlatForward(yaw);
	Vector3 right = FlatRight(yaw);
	Vector3 wish = Vector3Add(Vector3Scale(forward, input.moveInput.x), Vector3Scale(right, input.moveInput.y));
	wish.y = 0.0f;
	wish = NormalizeSafe(wish);

	bool sprinting = IsKeyDown(KEY_LEFT_SHIFT) && !coatMenu.open && input.moveInput.x > 0.0f;
	float speed = sprinting ? 8.2f : 5.5f;
	if (coatMenu.open) speed *= 0.42f;
	if (player.wardActive) speed *= 0.82f;

	player.velocity.x = wish.x * speed;
	player.velocity.z = wish.z * speed;

	if (input.moveInput.z > 0.0f && player.grounded && !coatMenu.open) {
		player.velocity.y = 6.2f;
		player.grounded = false;
	}

	player.velocity.y -= 16.0f * dt;

	Vector3 pos = player.position();
	pos.x += player.velocity.x * dt;
	pos.y += player.velocity.y * dt;
	pos.z += player.velocity.z * dt;

	if (pos.y <= EYE_HEIGHT) {
		pos.y = EYE_HEIGHT;
		player.velocity.y = 0.0f;
		player.grounded = true;
	}

	pos = resolveWorldCollision(pos);

	player.setPosition(pos);
	Vector3 look = ForwardFromYawPitch(yaw, pitch);
	player.camera.target = Vector3Add(player.camera.position, look);

	player.wardActive = IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && player.ward > 4.0f && player.wardCooldown <= 0.0f && !coatMenu.open;
	if (player.castCooldown > 0.0f) player.castCooldown -= dt;
}

void Game::updateEnemies(float dt) {
	Vector3 playerFeet{player.position().x, 0.0f, player.position().z};

	for (Enemy& enemy : enemies) {
		if (!enemy.alive) {
			if (!enemy.droppedLoot) {
				enemy.droppedLoot = true;
				ReagentType drop = static_cast<ReagentType>(static_cast<int>(enemy.kind) % REAGENT_COUNT);
				pickups.push_back(Pickup{drop, Vector3{enemy.position.x, 0.25f, enemy.position.z}, static_cast<float>(GetTime())});
			}
			continue;
		}

		if (enemy.attackCooldown > 0.0f) enemy.attackCooldown -= dt;
		if (enemy.snareTimer > 0.0f) enemy.snareTimer -= dt;
		if (enemy.fearTimer > 0.0f) enemy.fearTimer -= dt;
		if (enemy.poisonTimer > 0.0f) {
			enemy.poisonTimer -= dt;
			enemy.health -= enemy.poisonDps * dt;
			if (enemy.health <= 0.0f) enemy.alive = false;
		}

		Vector3 toPlayer = Vector3Subtract(playerFeet, enemy.position);
		toPlayer.y = 0.0f;
		Vector3 dir = NormalizeSafe(toPlayer);
		if (enemy.fearTimer > 0.0f) dir = Vector3Scale(dir, -1.0f);
		float moveSpeed = enemy.kind == EnemyKind::TorchMob ? 2.4f : 1.7f;
		if (enemy.snareTimer > 0.0f) moveSpeed *= 0.25f;
		if (enemy.kind != EnemyKind::TorchMob && DistanceXZ(enemy.position, playerFeet) < 5.0f && enemy.fearTimer <= 0.0f) {
			dir = Vector3Scale(dir, -0.7f);
		}
		enemy.position = resolveWorldCollision(Vector3Add(enemy.position, Vector3Scale(dir, moveSpeed * dt)));

		float attackRange = enemy.kind == EnemyKind::TorchMob ? 1.15f : 7.5f;
		if (enemy.telegraphTimer > 0.0f) enemy.telegraphTimer -= dt;
		if (DistanceXZ(enemy.position, playerFeet) <= attackRange && enemy.attackCooldown <= 0.0f && player.health > 0.0f && hasLineOfSight(Vector3Add(enemy.position, Vector3{0.0f, 0.9f, 0.0f}), player.position())) {
			if (enemy.kind == EnemyKind::TorchMob) {
				player.applyDamage(10.0f);
				enemy.attackCooldown = 0.85f;
			} else {
				Vector3 muzzle = Vector3Add(enemy.position, Vector3{0.0f, 1.0f, 0.0f});
				Vector3 aim = NormalizeSafe(Vector3Subtract(player.position(), muzzle));
				SpellStats shotStats;
				shotStats.damage = enemy.kind == EnemyKind::WardEngineer ? 12.0f : 8.0f;
				shotStats.radius = 0.35f;
				Effect shot{EffectKind::EnemyShot, {}, shotStats, muzzle, Vector3Scale(aim, enemy.kind == EnemyKind::WardEngineer ? 9.0f : 12.0f), 0.0f, 2.2f, 0.18f};
				shot.hostile = true;
				effects.push_back(shot);
				enemy.attackCooldown = enemy.kind == EnemyKind::WardEngineer ? 1.65f : 1.25f;
				enemy.telegraphTimer = 0.22f;
			}
		}
	}
}

void Game::updateEffects(float dt) {
	for (Effect& effect : effects) {
		if (!effect.active) continue;

		effect.age += dt;
		if (effect.age >= effect.lifetime) {
			effect.active = false;
			continue;
		}

		if (effect.kind == EffectKind::Projectile || effect.kind == EffectKind::EnemyShot) {
			if (effect.stats.seekStrength > 0.0f) {
				Enemy* nearest = nullptr;
				float nearestDistance = 9999.0f;
				for (Enemy& enemy : enemies) {
					if (!enemy.alive) continue;
					float distance = Vector3Distance(effect.position, Vector3Add(enemy.position, Vector3{0.0f, 0.9f, 0.0f}));
					if (distance < nearestDistance) {
						nearestDistance = distance;
						nearest = &enemy;
					}
				}
				if (nearest && nearestDistance < 12.0f) {
					Vector3 target = Vector3Add(nearest->position, Vector3{0.0f, 0.8f, 0.0f});
					Vector3 desired = Vector3Scale(NormalizeSafe(Vector3Subtract(target, effect.position)), effect.stats.speed);
					effect.velocity = Vector3Lerp(effect.velocity, desired, std::clamp(effect.stats.seekStrength * dt, 0.0f, 1.0f));
				}
			}

			effect.velocity.y -= 5.0f * dt;
			effect.position = Vector3Add(effect.position, Vector3Scale(effect.velocity, dt));
			bool hitObstacle = false;
			for (const Obstacle& obstacle : obstacles) {
				if (PointInsideObstacleXZ(effect.position, obstacle, effect.radius) && effect.position.y < obstacle.center.y + obstacle.size.y * 0.5f) {
					hitObstacle = true;
					break;
				}
			}
			if (hitObstacle) {
				if (effect.kind == EffectKind::Projectile) {
					triggerSpellImpact(effect.draft, effect.stats, effect.position);
				}
				effect.active = false;
				continue;
			}
			if (effect.position.y <= 0.18f) {
				if (effect.kind == EffectKind::Projectile) {
					triggerSpellImpact(effect.draft, effect.stats, Vector3{effect.position.x, 0.12f, effect.position.z});
				}
				effect.active = false;
				continue;
			}

			if (effect.kind == EffectKind::EnemyShot) {
				if (Vector3Distance(effect.position, player.position()) <= PLAYER_RADIUS + effect.radius + 0.25f) {
					player.applyDamage(effect.stats.damage);
					effect.active = false;
					continue;
				}
			} else {
				for (Enemy& enemy : enemies) {
					if (!enemy.alive) continue;
					if (Vector3Distance(effect.position, Vector3Add(enemy.position, Vector3{0.0f, 0.8f, 0.0f})) <= enemy.radius + effect.radius) {
						triggerSpellImpact(effect.draft, effect.stats, effect.position);
						effect.active = false;
						break;
					}
				}
			}
		} else if (effect.kind == EffectKind::Mine) {
			for (Enemy& enemy : enemies) {
				if (!enemy.alive) continue;
				if (DistanceXZ(effect.position, enemy.position) <= effect.stats.radius) {
					triggerSpellImpact(effect.draft, effect.stats, effect.position);
					effect.active = false;
					break;
				}
			}
		} else if (effect.kind == EffectKind::Hazard) {
			for (Enemy& enemy : enemies) {
				if (!enemy.alive) continue;
				float distance = DistanceXZ(effect.position, enemy.position);
				if (distance <= effect.stats.radius) {
					SpellStats tick = effect.stats;
					tick.damage = effect.stats.damage * 0.28f * dt;
					applySpellToEnemy(enemy, tick, distance);
				}
			}
		}
	}

	effects.erase(std::remove_if(effects.begin(), effects.end(), [](const Effect& effect) {
		return !effect.active;
	}), effects.end());
}

void Game::updatePickups(float dt) {
	(void)dt;
	Vector3 playerFeet{player.position().x, 0.0f, player.position().z};
	for (Pickup& pickup : pickups) {
		if (!pickup.active) continue;
		if (DistanceXZ(playerFeet, pickup.position) < 1.0f) {
			player.giveReagent(pickup.type, 1);
			pickup.active = false;
		}
	}

	pickups.erase(std::remove_if(pickups.begin(), pickups.end(), [](const Pickup& pickup) {
		return !pickup.active;
	}), pickups.end());
}

void Game::castCurrentSpell() {
	if (player.castCooldown > 0.0f) return;

	SpellDraft draft = player.draft;
	if (draft.base == BaseVector::Poppet || draft.reagentCount == 0) {
		draft.base = BaseVector::Poppet;
		draft.reagentCount = 0;
	} else if (!player.consumeDraftReagents()) {
		return;
	}

	SpellStats stats = CompileSpell(draft);
	Vector3 forward = ForwardFromYawPitch(yaw, pitch);
	Vector3 start = Vector3Add(player.position(), Vector3Scale(forward, 0.75f));
	start.y -= 0.12f;

	if (draft.base == BaseVector::Apple) {
		player.ward = std::min(player.maxWard, player.ward + stats.wardGain);
		triggerSpellImpact(draft, stats, Vector3{player.position().x, 0.2f, player.position().z});
		player.castCooldown = 0.55f;
		return;
	}

	if (draft.base == BaseVector::Spider) {
		effects.push_back(Effect{EffectKind::Mine, draft, stats, Vector3{player.position().x + forward.x * 1.15f, 0.08f, player.position().z + forward.z * 1.15f}, {}, 0.0f, stats.duration, 0.45f});
		player.castCooldown = 0.45f;
		return;
	}

	float speed = stats.speed;
	effects.push_back(Effect{EffectKind::Projectile, draft, stats, start, Vector3Scale(forward, speed), 0.0f, 3.0f, draft.base == BaseVector::Poppet ? 0.18f : 0.32f});
	player.castCooldown = draft.base == BaseVector::Poppet ? 0.22f : 0.42f;
}

void Game::triggerSpellImpact(const SpellDraft& draft, const SpellStats& stats, Vector3 position) {
	float radius = stats.radius;
	if (draft.base == BaseVector::Poppet) radius = 0.55f;

	for (Enemy& enemy : enemies) {
		if (!enemy.alive) continue;
		float distance = Vector3Distance(Vector3Add(enemy.position, Vector3{0.0f, 0.7f, 0.0f}), position);
		if (distance <= radius + enemy.radius) {
			applySpellToEnemy(enemy, stats, distance);
		}
	}

	effects.push_back(Effect{EffectKind::Burst, draft, stats, position, {}, 0.0f, 0.22f, radius});
	if (stats.lingering) {
		Effect hazard{EffectKind::Hazard, draft, stats, Vector3{position.x, 0.04f, position.z}, {}, 0.0f, std::max(2.0f, stats.duration), radius};
		effects.push_back(hazard);
	}
}

void Game::applySpellToEnemy(Enemy& enemy, const SpellStats& stats, float distance) {
	float falloff = std::clamp(1.0f - distance / std::max(stats.radius, 0.1f), 0.25f, 1.0f);
	enemy.health -= stats.damage * falloff;
	if (stats.snareSeconds > 0.0f) enemy.snareTimer = std::max(enemy.snareTimer, stats.snareSeconds);
	if (stats.fearSeconds > 0.0f) enemy.fearTimer = std::max(enemy.fearTimer, stats.fearSeconds);
	if (stats.poisonDps > 0.0f) {
		enemy.poisonDps = std::max(enemy.poisonDps, stats.poisonDps);
		enemy.poisonTimer = std::max(enemy.poisonTimer, 4.0f);
	}
	if (stats.wardGain > 0.0f) {
		player.ward = std::min(player.maxWard, player.ward + stats.wardGain * 0.16f);
	}
	if (enemy.health <= 0.0f) {
		enemy.alive = false;
	}
}

Vector3 Game::resolveWorldCollision(Vector3 position) const {
	position.x = std::clamp(position.x, -ARENA_LIMIT + PLAYER_RADIUS, ARENA_LIMIT - PLAYER_RADIUS);
	position.z = std::clamp(position.z, -ARENA_LIMIT + PLAYER_RADIUS, ARENA_LIMIT - PLAYER_RADIUS);

	for (const Obstacle& obstacle : obstacles) {
		if (!PointInsideObstacleXZ(position, obstacle, PLAYER_RADIUS)) continue;

		float leftPush = std::abs(position.x - (obstacle.center.x - obstacle.size.x * 0.5f - PLAYER_RADIUS));
		float rightPush = std::abs(position.x - (obstacle.center.x + obstacle.size.x * 0.5f + PLAYER_RADIUS));
		float backPush = std::abs(position.z - (obstacle.center.z - obstacle.size.z * 0.5f - PLAYER_RADIUS));
		float frontPush = std::abs(position.z - (obstacle.center.z + obstacle.size.z * 0.5f + PLAYER_RADIUS));
		float minPush = std::min(std::min(leftPush, rightPush), std::min(backPush, frontPush));

		if (minPush == leftPush) {
			position.x = obstacle.center.x - obstacle.size.x * 0.5f - PLAYER_RADIUS;
		} else if (minPush == rightPush) {
			position.x = obstacle.center.x + obstacle.size.x * 0.5f + PLAYER_RADIUS;
		} else if (minPush == backPush) {
			position.z = obstacle.center.z - obstacle.size.z * 0.5f - PLAYER_RADIUS;
		} else {
			position.z = obstacle.center.z + obstacle.size.z * 0.5f + PLAYER_RADIUS;
		}
	}

	return position;
}

bool Game::hasLineOfSight(Vector3 from, Vector3 to) const {
	for (const Obstacle& obstacle : obstacles) {
		for (int i = 1; i < 12; ++i) {
			float t = static_cast<float>(i) / 12.0f;
			Vector3 sample = Vector3Lerp(from, to, t);
			if (sample.y <= obstacle.center.y + obstacle.size.y * 0.5f && PointInsideObstacleXZ(sample, obstacle, 0.05f)) {
				return false;
			}
		}
	}

	return true;
}

void Game::draw() {
	BeginDrawing();
	ClearBackground(Color{5, 8, 9, 255});
	drawWorld();
	drawHands();

	int aliveEnemies = 0;
	for (const Enemy& enemy : enemies) {
		if (enemy.alive) aliveEnemies++;
	}

	hud.draw(player, aliveEnemies, screenWidth, screenHeight);
	coatMenu.draw(player, screenWidth, screenHeight);
	drawFlowOverlay();
	EndDrawing();
}

void Game::drawWorld() {
	BeginMode3D(player.camera);
	DrawPlane(Vector3{0.0f, -0.01f, 0.0f}, Vector2{ARENA_LIMIT * 2.2f, ARENA_LIMIT * 2.2f}, Color{10, 18, 14, 255});
	DrawGrid(36, 1.0f);
	DrawArenaFence();

	for (const Obstacle& obstacle : obstacles) {
		DrawCubeWires(obstacle.center, obstacle.size.x, obstacle.size.y, obstacle.size.z, Color{110, 135, 105, 255});
	}

	for (const Pickup& pickup : pickups) {
		const ReagentDefinition& reagent = GetReagentDefinition(pickup.type);
		float bob = std::sin(static_cast<float>(GetTime()) * 3.0f + pickup.bobSeed) * 0.08f;
		Vector3 pos = Vector3{pickup.position.x, pickup.position.y + bob, pickup.position.z};
		DrawCircle3D(pos, 0.25f, Vector3{1.0f, 0.0f, 0.0f}, 90.0f, reagent.color);
		DrawCircle3D(pos, 0.25f, Vector3{0.0f, 0.0f, 1.0f}, 90.0f, reagent.color);
		DrawLine3D(Vector3{pos.x, pos.y - 0.4f, pos.z}, Vector3{pos.x, pos.y + 0.4f, pos.z}, reagent.color);
	}

	for (const Effect& effect : effects) {
		if (effect.kind == EffectKind::Projectile) {
			DrawSphereWires(effect.position, effect.radius, 8, 4, Color{235, 245, 190, 255});
		} else if (effect.kind == EffectKind::EnemyShot) {
			DrawSphereWires(effect.position, effect.radius, 8, 4, Color{255, 95, 60, 255});
			DrawLine3D(effect.position, Vector3Subtract(effect.position, Vector3Scale(NormalizeSafe(effect.velocity), 0.65f)), Color{255, 95, 60, 255});
		} else if (effect.kind == EffectKind::Mine) {
			DrawCircle3D(effect.position, effect.stats.radius, Vector3{1.0f, 0.0f, 0.0f}, 90.0f, Color{90, 255, 120, 255});
			DrawSphereWires(Vector3{effect.position.x, 0.16f, effect.position.z}, 0.18f, 8, 4, Color{90, 255, 120, 255});
		} else if (effect.kind == EffectKind::Hazard) {
			DrawCircle3D(effect.position, effect.stats.radius, Vector3{1.0f, 0.0f, 0.0f}, 90.0f, Color{40, 40, 55, 255});
			DrawCircle3D(Vector3{effect.position.x, 0.03f, effect.position.z}, effect.stats.radius * 0.75f, Vector3{1.0f, 0.0f, 0.0f}, 90.0f, Color{80, 80, 110, 255});
		} else if (effect.kind == EffectKind::Burst) {
			float t = effect.age / effect.lifetime;
			DrawSphereWires(effect.position, effect.radius * (0.35f + t), 18, 8, Color{245, 230, 145, static_cast<unsigned char>(220 * (1.0f - t))});
		}
	}

	for (const Enemy& enemy : enemies) {
		if (!enemy.alive) continue;
		Color color = EnemyColor(enemy.kind);
		if (enemy.fearTimer > 0.0f) color = Color{210, 120, 255, 255};
		if (enemy.snareTimer > 0.0f) DrawCircle3D(enemy.position, 0.75f, Vector3{1.0f, 0.0f, 0.0f}, 90.0f, Color{70, 240, 90, 255});
		if (enemy.telegraphTimer > 0.0f) {
			DrawLine3D(Vector3Add(enemy.position, Vector3{0.0f, 1.0f, 0.0f}), player.position(), Color{255, 80, 60, 190});
		}
		DrawBillboardGlyph(player.camera, enemy.position, enemy.radius * 1.8f, enemy.kind == EnemyKind::WardEngineer ? 1.9f : 1.55f, color, true);
		float healthT = std::clamp(enemy.health / enemy.maxHealth, 0.0f, 1.0f);
		DrawLine3D(Vector3{enemy.position.x - 0.45f, 1.9f, enemy.position.z}, Vector3{enemy.position.x - 0.45f + 0.9f * healthT, 1.9f, enemy.position.z}, Color{255, 80, 70, 255});
	}

	if (player.wardActive) {
		Vector3 wardPos = Vector3Add(player.position(), Vector3Scale(ForwardFromYawPitch(yaw, 0.0f), 0.45f));
		DrawSphereWires(wardPos, 0.85f, 16, 8, Color{110, 220, 255, 150});
	}

	EndMode3D();
}

void Game::drawHands() const {
	int y = screenHeight - 102;
	Color hand = Color{230, 245, 220, 255};
	Color shadow = Color{8, 12, 10, 200};
	DrawRectangle(0, screenHeight - 132, screenWidth, 132, Color{0, 0, 0, 80});

	Vector2 leftPalm{screenWidth * 0.33f, static_cast<float>(y)};
	Vector2 rightPalm{screenWidth * 0.67f, static_cast<float>(y)};
	DrawCircleV(leftPalm, 28.0f, shadow);
	DrawCircleLines(static_cast<int>(leftPalm.x), static_cast<int>(leftPalm.y), 28.0f, hand);
	DrawCircleV(rightPalm, 28.0f, shadow);
	DrawCircleLines(static_cast<int>(rightPalm.x), static_cast<int>(rightPalm.y), 28.0f, hand);

	for (int i = 0; i < 4; ++i) {
		DrawLineEx(Vector2{leftPalm.x - 20.0f + i * 10.0f, leftPalm.y - 18.0f}, Vector2{leftPalm.x - 34.0f + i * 8.0f, leftPalm.y - 54.0f}, 3.0f, hand);
		DrawLineEx(Vector2{rightPalm.x + 20.0f - i * 10.0f, rightPalm.y - 18.0f}, Vector2{rightPalm.x + 34.0f - i * 8.0f, rightPalm.y - 54.0f}, 3.0f, hand);
	}

	if (coatMenu.open) {
		DrawLineEx(Vector2{leftPalm.x, leftPalm.y - 12.0f}, Vector2{screenWidth * 0.5f - 70.0f, screenHeight - 250.0f}, 4.0f, Color{255, 230, 130, 255});
		DrawLineEx(Vector2{rightPalm.x, rightPalm.y - 12.0f}, Vector2{screenWidth * 0.5f + 70.0f, screenHeight - 250.0f}, 4.0f, Color{255, 230, 130, 255});
	}
}

void Game::drawFlowOverlay() const {
	if (flowState == GameFlowState::Playing && waveMessageTimer <= 0.0f) return;

	if (flowState == GameFlowState::Playing) {
		DrawRectangle(screenWidth / 2 - 150, 92, 300, 42, Color{6, 11, 9, 205});
		DrawRectangleLines(screenWidth / 2 - 150, 92, 300, 42, Color{230, 245, 220, 255});
		DrawText(TextFormat("DISTRICT WAVE %d / %d", wave + 1, FINAL_WAVE), screenWidth / 2 - 104, 105, 18, Color{255, 220, 120, 255});
		return;
	}

	DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0, 190});
	if (flowState == GameFlowState::Victory) {
		DrawText("SALEM BREAKS BEFORE YOU", screenWidth / 2 - 205, screenHeight / 2 - 34, 30, Color{210, 255, 190, 255});
		DrawText("R restart the combat slice", screenWidth / 2 - 116, screenHeight / 2 + 10, 18, Color{230, 245, 220, 255});
	} else {
		DrawText("THE SALEM MOB HAS YOU", screenWidth / 2 - 170, screenHeight / 2 - 34, 28, Color{255, 80, 70, 255});
		DrawText("R restart the combat slice", screenWidth / 2 - 116, screenHeight / 2 + 10, 18, Color{230, 245, 220, 255});
	}
}
