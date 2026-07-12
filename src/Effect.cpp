#include <algorithm>
#include <cmath>
#include "Effect.hxx"
#include "Enemy.hxx"
#include "Player.hxx"
#include <raymath.h>

namespace {
	Color spellTint(const SpellPreview& spell) {
		if (spell.rotDamagePerSecond > 0.0f) {
			return Color {96, 210, 124, 220};
		}
		if (spell.fearDuration > 0.0f) {
			return Color {190, 120, 250, 220};
		}
		if (spell.snareDuration > 0.0f) {
			return Color {124, 240, 140, 220};
		}
		if (spell.wardGain > 0.0f) {
			return Color {120, 210, 255, 220};
		}
		return Color {255, 196, 120, 220};
	}
}

Effect::Effect()
	: Object({}, 0.25f) {
}

Effect Effect::MakeProjectile(Vector3 position, Vector3 direction, const SpellPreview& spell) {
	Effect effect;
	effect.kind = EffectKind::Projectile;
	effect.position = position;
	effect.direction = Vector3Normalize(direction);
	effect.spell = spell;
	effect.speed = spell.projectileSpeed;
	effect.remainingTime = spell.lifetime;
	effect.pulseRadius = spell.radius;
	return effect;
}

Effect Effect::MakePulse(Vector3 position, const SpellPreview& spell) {
	Effect effect;
	effect.kind = EffectKind::Pulse;
	effect.position = position;
	effect.spell = spell;
	effect.remainingTime = 0.22f;
	effect.pulseRadius = spell.radius;
	return effect;
}

Effect Effect::MakeMine(Vector3 position, const SpellPreview& spell) {
	Effect effect;
	effect.kind = EffectKind::Mine;
	effect.position = position;
	effect.spell = spell;
	effect.remainingTime = spell.lifetime;
	effect.triggerRadius = spell.radius;
	effect.pulseRadius = spell.radius;
	effect.armDelay = 0.25f;
	return effect;
}

Effect Effect::MakeHazard(Vector3 position, const SpellPreview& spell, float radius, float duration) {
	Effect effect;
	effect.kind = EffectKind::Hazard;
	effect.position = position;
	effect.spell = spell;
	effect.remainingTime = duration;
	effect.pulseRadius = radius;
	return effect;
}

void Effect::update(float dt) {
	remainingTime = std::max(0.0f, remainingTime - dt);
}

void Effect::tick(float dt, Player& player, std::vector<Enemy>& enemies, const std::vector<BoundingBox>& blockers) {
	if (!active) {
		return;
	}

	update(dt);
	switch (kind) {
		case EffectKind::Projectile: {
			if (spell.seekStrength > 0.0f) {
				Vector3 targetDir = nearestEnemyDirection(enemies);
				if (Vector3Length(targetDir) > 0.01f) {
					direction = Vector3Normalize(Vector3Lerp(direction, targetDir, std::min(0.9f, spell.seekStrength * dt * 3.0f)));
				}
			}

			position = Vector3Add(position, Vector3Scale(direction, speed * dt));
			bool hitWall = false;
			collideWithWorld(blockers, hitWall);

			bool hitEnemy = false;
			for (Enemy& enemy : enemies) {
				if (!enemy.isAlive()) {
					continue;
				}
				if (Vector3Distance(position, enemy.getPosition()) <= enemy.getCollisionRadius() + 0.35f) {
					hitEnemy = true;
					break;
				}
			}

			if (hitWall || hitEnemy || remainingTime <= 0.0f) {
				detonate(enemies, player, spell.base != BaseVectorType::Poppet && (spell.rotDamagePerSecond > 0.0f || spell.snareDuration > 0.0f));
			}
			break;
		}
		case EffectKind::Pulse:
			if (!appliedPulse) {
				applyArea(enemies, player);
				appliedPulse = true;
			}
			if (remainingTime <= 0.0f) {
				active = false;
			}
			break;
		case EffectKind::Mine: {
			if (spell.seekStrength > 0.0f && armDelay <= 0.0f) {
				Vector3 crawl = nearestEnemyDirection(enemies);
				crawl.y = 0.0f;
				if (Vector3Length(crawl) > 0.01f) {
					position = Vector3Add(position, Vector3Scale(Vector3Normalize(crawl), dt * spell.seekStrength));
				}
			}

			armDelay = std::max(0.0f, armDelay - dt);
			bool triggeredMine = remainingTime <= 0.0f;
			if (armDelay <= 0.0f) {
				for (const Enemy& enemy : enemies) {
					if (!enemy.isAlive()) {
						continue;
					}
					if (Vector3Distance(position, enemy.getPosition()) <= triggerRadius + enemy.getCollisionRadius()) {
						triggeredMine = true;
						break;
					}
				}
			}

			if (triggeredMine) {
				detonate(enemies, player, true);
			}
			break;
		}
		case EffectKind::Hazard: {
			SpellPreview seep = spell;
			seep.damage = spell.rotDamagePerSecond * dt + spell.damage * 0.08f * dt;
			seep.rotDamagePerSecond = 0.0f;
			seep.rotDuration = 0.0f;
			seep.fearDuration = 0.0f;
			seep.wardGain = 0.0f;
			for (Enemy& enemy : enemies) {
				if (!enemy.isAlive()) {
					continue;
				}
				if (Vector3Distance(position, enemy.getPosition()) <= pulseRadius + enemy.getCollisionRadius()) {
					enemy.applySpell(seep, position);
				}
			}
			if (remainingTime <= 0.0f) {
				active = false;
			}
			break;
		}
	}
}

void Effect::draw3D(const Camera3D&) const {
	if (!active) {
		return;
	}

	const Color tint = spellTint(spell);
	switch (kind) {
		case EffectKind::Projectile:
			DrawSphereWires(position, 0.18f, 8, 8, tint);
			DrawLine3D(position, Vector3Subtract(position, Vector3Scale(direction, 0.55f)), tint);
			break;
		case EffectKind::Pulse:
			DrawCircle3D(position, pulseRadius, Vector3 {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(tint, 0.35f));
			DrawSphereWires(position, 0.35f, 8, 8, tint);
			break;
		case EffectKind::Mine:
			DrawSphereWires(Vector3 {position.x, position.y + 0.18f, position.z}, 0.25f, 8, 8, tint);
			DrawCircle3D(position, triggerRadius, Vector3 {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(tint, 0.4f));
			break;
		case EffectKind::Hazard:
			DrawCircle3D(position, pulseRadius, Vector3 {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(tint, 0.28f));
			DrawCircle3D(position, pulseRadius * 0.65f, Vector3 {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(tint, 0.16f));
			break;
	}
}

bool Effect::hasExpired() const {
	return !active;
}

void Effect::detonate(std::vector<Enemy>& enemies, Player& player, bool allowHazard) {
	applyArea(enemies, player);
	if (allowHazard && spell.lingerDuration > 0.0f) {
		kind = EffectKind::Hazard;
		remainingTime = spell.lingerDuration;
		pulseRadius = spell.radius * 0.85f;
		appliedPulse = true;
		triggered = true;
		return;
	}

	active = false;
}

void Effect::applyArea(std::vector<Enemy>& enemies, Player& player) {
	int affected = 0;
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) {
			continue;
		}
		if (Vector3Distance(position, enemy.getPosition()) <= pulseRadius + enemy.getCollisionRadius()) {
			enemy.applySpell(spell, position);
			affected += 1;
		}
	}

	if (affected > 0 && spell.wardGain > 0.0f) {
		player.addWard(spell.wardGain * std::min(1.5f, 0.5f + affected * 0.35f));
	}
}

Vector3 Effect::nearestEnemyDirection(const std::vector<Enemy>& enemies) const {
	float closestDistance = 9999.0f;
	Vector3 best {};
	for (const Enemy& enemy : enemies) {
		if (!enemy.isAlive()) {
			continue;
		}
		Vector3 delta = Vector3Subtract(enemy.getPosition(), position);
		const float distance = Vector3Length(delta);
		if (distance < closestDistance) {
			closestDistance = distance;
			best = delta;
		}
	}

	if (closestDistance < 9999.0f && Vector3Length(best) > 0.001f) {
		return Vector3Normalize(best);
	}

	return Vector3 {};
}

void Effect::collideWithWorld(const std::vector<BoundingBox>& blockers, bool& hitWall) {
	for (const BoundingBox& box : blockers) {
		if (CheckCollisionBoxSphere(box, position, 0.16f)) {
			hitWall = true;
			return;
		}
	}

	if (position.x < -24.0f || position.x > 24.0f || position.z < -35.0f || position.z > 35.0f || position.y < 0.1f || position.y > 6.0f) {
		hitWall = true;
	}
}
