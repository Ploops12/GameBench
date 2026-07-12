#include "Game.h"

#include <algorithm>

Game::Game() {
	spawnEncounter();
	scenery.emplace_back(Vector3{-7.0f, 2.0f, 0.0f}, Vector3{2.0f, 4.0f, 10.0f}, Color{45, 54, 68, 255});
	scenery.emplace_back(Vector3{7.0f, 2.0f, -2.0f}, Vector3{2.0f, 4.0f, 10.0f}, Color{58, 47, 65, 255});
	scenery.emplace_back(Vector3{0.0f, 2.0f, -12.0f}, Vector3{16.0f, 4.0f, 2.0f}, Color{48, 57, 65, 255});
	pickups = {
		{{-3.5f, 0.45f, 4.0f}, ReagentType::WidowNettle, true},
		{{3.2f, 0.45f, 1.0f}, ReagentType::MothDust, true},
		{{-4.0f, 0.45f, -5.0f}, ReagentType::BogMarrow, true},
		{{4.0f, 0.45f, -7.0f}, ReagentType::BlackWax, true}
	};
}

void Game::spawnEncounter() {
	enemies.emplace_back(Vector3{-3.0f, 1.0f, -5.0f}, 0);
	enemies.emplace_back(Vector3{3.2f, 1.0f, -7.5f}, 0);
	enemies.emplace_back(Vector3{0.0f, 1.0f, -10.0f}, 1);
}

void Game::update(float dt) {
	if (!started) {
		if (mainMenu.update()) started = true;
		return;
	}
	if (won) return;
	InputHandler::InputState state = input.poll();
	const bool coatOpen = coatMenu.update(player);
	const Vector3 previousPosition = player.getPosition();
	player.update(dt, state, coatOpen);
	player.setPosition(Vector3{std::clamp(player.getPosition().x, -5.8f, 5.8f), player.getPosition().y, std::clamp(player.getPosition().z, -10.8f, 6.2f)});
	for (const Scenery &wall : scenery) if (wall.collides(player.getPosition(), player.getRadius())) { player.setPosition(previousPosition); break; }
	if (IsKeyPressed(KEY_R) && !coatOpen) player.activateWard();
	for (WorldPickup &pickup : pickups) {
		if (pickup.active && Vector3Distance(player.getPosition(), pickup.position) < 1.0f) {
			player.addReagent(pickup.type);
			pickup.active = false;
			message = TextFormat("Collected %s: %s", getReagentDefinition(pickup.type).name, getReagentDefinition(pickup.type).effect);
			messageTimer = 2.5f;
		}
	}
	for (Enemy &enemy : enemies) {
		enemy.update(dt, player.getPosition(), player.isVulnerable());
		if (enemy.canAttack() && Vector3Distance(enemy.getPosition(), player.getPosition()) < 1.65f) {
			player.damage(enemy.getVariant() == 1 ? 14 : 9);
			enemy.consumeAttack();
		}
	}
	for (CurseEffect &effect : effects) {
		if (effect.stats.seeking > 0.0f) {
			const Enemy *nearest = nullptr;
			float nearestDistance = 1000.0f;
			for (const Enemy &enemy : enemies) if (!enemy.isDead()) {
				const float distance = Vector3Distance(effect.position, enemy.getPosition());
				if (distance < nearestDistance) { nearest = &enemy; nearestDistance = distance; }
			}
			if (nearest != nullptr) effect.velocity = Vector3Scale(Vector3Normalize(Vector3Subtract(nearest->getPosition(), effect.position)), 8.0f);
		}
		effect.update(dt);
		if (!effect.active) continue;
		if (effect.kind == EffectKind::Mine) {
			for (Enemy &enemy : enemies) if (!enemy.isDead() && Vector3Distance(effect.position, enemy.getPosition()) < 1.1f + effect.stats.radius) {
				enemy.takeDamage(effect.stats.damage, effect.stats.snare);
				effect.active = false;
			}
		} else if (effect.kind == EffectKind::Bolt) {
			for (Enemy &enemy : enemies) if (!enemy.isDead() && Vector3Distance(effect.position, enemy.getPosition()) < 0.75f) {
				const Vector3 hit = enemy.getPosition();
				enemy.takeDamage(effect.stats.damage, effect.stats.snare);
				if (effect.stats.panic) enemy.panic(effect.stats.duration);
				if (effect.stats.rot) enemy.applyRot(std::max(1, effect.stats.damage / 3), effect.stats.duration);
				if (effect.stats.wardGain > 0) player.gainWard(effect.stats.wardGain);
				if (effect.stats.radius > 0.0f) for (Enemy &splash : enemies) if (!splash.isDead() && Vector3Distance(hit, splash.getPosition()) < effect.stats.radius + 0.8f) splash.takeDamage(effect.stats.damage / 2, effect.stats.snare);
				effect.active = false;
			}
		}
	}
	for (Enemy &enemy : enemies) if (enemy.isDead() && enemy.takeDrop()) {
		const ReagentType drop = static_cast<ReagentType>((enemy.getVariant() + 3) % REAGENT_COUNT);
		pickups.push_back(WorldPickup{enemy.getPosition(), drop, true});
	}
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !coatOpen) castSpell();
	if (messageTimer > 0.0f) messageTimer -= dt;
	if (player.getHealth() <= 0) { player = Player{}; message = "The hunters got you. The witch rises again."; messageTimer = 3.0f; }
	if (std::all_of(enemies.begin(), enemies.end(), [](const Enemy &enemy) { return enemy.isDead(); })) { won = true; message = "SALEM BENDS. Press ESC to release the cursor."; }
}

void Game::castSpell() {
	const SpellStats stats = player.spell().stats();
	const Vector3 start = Vector3Add(player.getPosition(), Vector3{0.0f, 0.1f, 0.0f});
	const BaseVector base = player.spell().getBase();
	if (base == BaseVector::Apple) {
		player.gainWard(stats.wardGain);
		message = "The apple is eaten. A ward grows under the skin.";
	} else {
		CurseEffect effect;
		effect.stats = stats;
		effect.position = start;
		if (base == BaseVector::Spider) {
			effect.kind = EffectKind::Mine;
			effect.position = Vector3Add(start, Vector3Scale(player.forward(), 2.0f));
			effect.velocity = {};
			effect.life = 8.0f + stats.duration;
			message = "A thorn mine takes root.";
		} else {
			effect.kind = EffectKind::Bolt;
			effect.velocity = Vector3Scale(player.forward(), stats.seeking > 0.0f ? 8.0f : 13.0f);
			effect.life = 2.0f;
			message = base == BaseVector::Poppet ? "The poppet twitches and bites." : "The bottle breaks into a moving curse.";
		}
		effects.push_back(effect);
	}
	player.consumeSpellReagents();
}

void Game::drawWorld() const {
	DrawPlane(Vector3{0.0f, 0.0f, -2.0f}, Vector2{16.0f, 24.0f}, Color{24, 31, 34, 255});
	DrawGrid(24, 1.0f);
	for (const Scenery &item : scenery) item.draw();
	for (const WorldPickup &pickup : pickups) if (pickup.active) {
		const ReagentDefinition &info = getReagentDefinition(pickup.type);
		DrawCylinder(pickup.position, 0.18f, 0.18f, 0.4f, 6, info.color);
		DrawCircle3D(Vector3Add(pickup.position, Vector3{0, 0.25f, 0}), 0.28f, Vector3{0, 1, 0}, 0.0f, Fade(info.color, 0.4f));
	}
	for (const Enemy &enemy : enemies) enemy.draw();
	for (const CurseEffect &effect : effects) effect.draw();
}

void Game::draw() const {
	if (!started) {
		mainMenu.draw();
		return;
	}
	ClearBackground(Color{16, 22, 31, 255});
	BeginMode3D(player.getCamera());
	drawWorld();
	EndMode3D();
	player.drawHands();
	hud.draw(player, coatMenu.isOpen(), static_cast<int>(std::count_if(enemies.begin(), enemies.end(), [](const Enemy &enemy) { return !enemy.isDead(); })));
	if (messageTimer > 0.0f) DrawText(message.c_str(), 24, 24, 16, Color{255, 235, 180, 255});
	if (won) {
		DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.45f));
		DrawText("THE HEX HOLDS", GetScreenWidth() / 2 - 130, GetScreenHeight() / 2 - 32, 30, Color{255, 230, 150, 255});
	}
	coatMenu.draw(player);
}
