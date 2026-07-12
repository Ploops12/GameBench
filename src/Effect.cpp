#include "Effect.hxx"

#include <algorithm>
#include <cmath>

#include <raymath.h>

#include "Enemy.hxx"

namespace {
constexpr float BottleSpeed = 17.0f;
constexpr float NeedleSpeed = 29.0f;
constexpr float ProjectileGravity = 10.5f;
constexpr float BottleThrowLift = 0.45f;

Color effectColor(const SpellStats& spell) {
	if (spell.poisonDuration > 0.0f) {
		return Color {244, 187, 62, 255};
	}
	if (spell.snareDuration > 0.0f) {
		return Color {133, 245, 105, 255};
	}
	if (spell.fearDuration > 0.0f) {
		return Color {198, 122, 239, 255};
	}
	return Color {101, 226, 226, 255};
}

float effectiveRadius(const SpellStats& spell) {
	return std::max(0.20f, spell.radius);
}
}

Effect Effect::makeThrownBottle(Vector3 position, Vector3 direction, const SpellStats& spell) {
	Vector3 velocity = Vector3Scale(Vector3Normalize(direction), BottleSpeed);
	velocity.y += BottleThrowLift;
	return Effect(EffectKind::ThrownBottle, position, velocity, spell, 2.1f);
}

Effect Effect::makePoppetNeedle(Vector3 position, Vector3 direction, const SpellStats& spell) {
	return Effect(EffectKind::PoppetNeedle, position,
		Vector3Scale(Vector3Normalize(direction), NeedleSpeed), spell, 0.95f);
}

Effect Effect::makePlacedSpider(Vector3 position, const SpellStats& spell) {
	position.y = 0.06f;
	return Effect(EffectKind::PlacedSpider, position, {}, spell, 16.0f, 0.42f);
}

Effect Effect::makeBurst(Vector3 position, const SpellStats& spell) {
	return Effect(EffectKind::Burst, position, {}, spell, 0.16f);
}

Effect Effect::makeSelfHexAura(Vector3 position, const SpellStats& spell) {
	return Effect(EffectKind::SelfHexAura, position, {}, spell, 0.48f);
}

Effect::Effect(EffectKind initialKind, Vector3 initialPosition, Vector3 initialVelocity,
	SpellStats initialSpell, float initialRemainingLife, float initialArmingTime)
	: kind(initialKind), position(initialPosition), velocity(initialVelocity), spell(initialSpell),
	  remainingLife(initialRemainingLife), armingTime(initialArmingTime) {
}

void Effect::update(float deltaTime, const std::vector<BoundingBox>& solidBounds, std::vector<Enemy>& enemies) {
	if (!active) {
		return;
	}

	visualAge += deltaTime;
	remainingLife -= deltaTime;

	if (kind == EffectKind::Burst) {
		if (!burstApplied) {
			applyAt(enemies, 1.0f);
			burstApplied = true;
			if (spell.lingeringDuration > 0.0f) {
				kind = EffectKind::LingeringHazard;
				position.y = 0.06f;
				remainingLife = spell.lingeringDuration;
				tickTimer = 0.0f;
				return;
			}
		}
		if (remainingLife <= 0.0f) {
			active = false;
		}
		return;
	}

	if (kind == EffectKind::LingeringHazard) {
		tickTimer -= deltaTime;
		if (tickTimer <= 0.0f) {
			applyAt(enemies, 0.28f);
			tickTimer = 0.36f;
		}
		if (remainingLife <= 0.0f) {
			active = false;
		}
		return;
	}

	if (kind == EffectKind::SelfHexAura) {
		if (remainingLife <= 0.0f) {
			active = false;
		}
		return;
	}

	if (kind == EffectKind::PlacedSpider) {
		armingTime = std::max(0.0f, armingTime - deltaTime);
		if (armingTime <= 0.0f) {
			for (const Enemy& enemy : enemies) {
				if (enemy.isAlive() && CheckCollisionSpheres(position, effectiveRadius(spell),
					enemy.getTargetPosition(), enemy.getCollisionRadius())) {
					explode(enemies);
					return;
				}
			}
		}
		if (remainingLife <= 0.0f) {
			active = false;
		}
		return;
	}

	if (spell.homingStrength > 0.0f) {
		if (Enemy* target = findHomingTarget(enemies); target != nullptr) {
			Vector3 desired = Vector3Normalize(Vector3Subtract(target->getTargetPosition(), position));
			Vector3 current = Vector3Normalize(velocity);
			const float blend = std::min(1.0f, spell.homingStrength * deltaTime * 5.0f);
			const float speed = Vector3Length(velocity);
			velocity = Vector3Scale(Vector3Normalize(Vector3Add(
				Vector3Scale(current, 1.0f - blend), Vector3Scale(desired, blend))), speed);
		}
	}

	if (kind == EffectKind::ThrownBottle) {
		velocity.y -= ProjectileGravity * deltaTime;
	}
	const Vector3 previousPosition = position;
	const Vector3 travel = Vector3Scale(velocity, deltaTime);
	const float travelDistance = Vector3Length(travel);
	position = Vector3Add(previousPosition, travel);

	bool hitSomething = false;
	Vector3 impactPosition = position;
	float closestImpact = travelDistance;
	if (travelDistance > 0.0001f) {
		const Ray travelRay {previousPosition, Vector3Scale(travel, 1.0f / travelDistance)};
		for (const BoundingBox& bounds : solidBounds) {
			const RayCollision collision = GetRayCollisionBox(travelRay, bounds);
			if (collision.hit && collision.distance <= closestImpact) {
				hitSomething = true;
				closestImpact = collision.distance;
				impactPosition = collision.point;
			}
		}
		for (const Enemy& enemy : enemies) {
			if (!enemy.isAlive()) {
				continue;
			}

			const float hitRadius = enemy.getCollisionRadius() + (kind == EffectKind::ThrownBottle ? 0.18f : 0.08f);
			const RayCollision collision = GetRayCollisionSphere(travelRay, enemy.getTargetPosition(), hitRadius);
			if (collision.hit && collision.distance <= closestImpact) {
				hitSomething = true;
				closestImpact = collision.distance;
				impactPosition = collision.point;
			}
		}
	}

	if (hitSomething) {
		position = impactPosition;
	} else {
		hitSomething = touchesWorld(solidBounds) || position.y <= 0.05f;
	}

	if (hitSomething || remainingLife <= 0.0f) {
		explode(enemies);
	}
}

void Effect::draw(const Camera3D&) const {
	if (!active) {
		return;
	}

	const Color color = effectColor(spell);
	const float pulse = 0.85f + sinf(visualAge * 11.0f) * 0.15f;
	if (kind == EffectKind::ThrownBottle || kind == EffectKind::PoppetNeedle) {
		DrawSphereWires(position, kind == EffectKind::ThrownBottle ? 0.18f : 0.08f, 6, 6, color);
		DrawLine3D(position, Vector3Subtract(position, Vector3Scale(Vector3Normalize(velocity), 0.35f)), color);
		return;
	}

	if (kind == EffectKind::PlacedSpider) {
		const Color mineColor = armingTime > 0.0f ? Color {color.r, color.g, color.b, 130} : color;
		DrawCircle3D(position, effectiveRadius(spell) * 0.45f, Vector3 {1.0f, 0.0f, 0.0f}, 90.0f, mineColor);
		DrawSphereWires(Vector3Add(position, Vector3 {0.0f, 0.12f, 0.0f}), 0.14f, 5, 5, mineColor);
		for (int leg = 0; leg < 4; ++leg) {
			const float angle = static_cast<float>(leg) * PI * 0.5f + visualAge * 1.4f;
			DrawLine3D(position, Vector3Add(position, Vector3 {cosf(angle) * 0.3f, 0.02f, sinf(angle) * 0.3f}), mineColor);
		}
		return;
	}

	const float radius = effectiveRadius(spell) * pulse;
	const unsigned char alpha = static_cast<unsigned char>(kind == EffectKind::LingeringHazard ? 115 : 220);
	const Color transparent {color.r, color.g, color.b, alpha};
	DrawSphereWires(position, radius, 8, 8, transparent);
	DrawCircle3D(Vector3 {position.x, 0.035f, position.z}, radius, Vector3 {1.0f, 0.0f, 0.0f}, 90.0f, transparent);
	if (kind == EffectKind::LingeringHazard) {
		DrawCircle3D(Vector3 {position.x, 0.055f, position.z}, radius * 0.62f,
			Vector3 {1.0f, 0.0f, 0.0f}, 90.0f, transparent);
	}
}

bool Effect::isActive() const {
	return active;
}

EffectKind Effect::getKind() const {
	return kind;
}

const Vector3& Effect::getPosition() const {
	return position;
}

void Effect::explode(std::vector<Enemy>& enemies) {
	applyAt(enemies, 1.0f);
	burstApplied = true;
	if (spell.lingeringDuration > 0.0f) {
		kind = EffectKind::LingeringHazard;
		position.y = 0.06f;
		remainingLife = spell.lingeringDuration;
		tickTimer = 0.0f;
		velocity = {};
		return;
	}

	kind = EffectKind::Burst;
	remainingLife = 0.16f;
	velocity = {};
}

void Effect::applyAt(std::vector<Enemy>& enemies, float strength) {
	const float radius = effectiveRadius(spell);
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) {
			continue;
		}

		if (CheckCollisionSpheres(position, radius, enemy.getTargetPosition(), enemy.getCollisionRadius())) {
			enemy.applyHex(spell, strength);
		}
	}
}

Enemy* Effect::findHomingTarget(std::vector<Enemy>& enemies) const {
	Enemy* closest = nullptr;
	float closestDistance = 18.0f;
	for (Enemy& enemy : enemies) {
		if (!enemy.isAlive()) {
			continue;
		}

		const float distance = Vector3Distance(position, enemy.getTargetPosition());
		if (distance < closestDistance) {
			closest = &enemy;
			closestDistance = distance;
		}
	}

	return closest;
}

bool Effect::touchesWorld(const std::vector<BoundingBox>& solidBounds) const {
	for (const BoundingBox& bounds : solidBounds) {
		if (CheckCollisionBoxSphere(bounds, position, 0.16f)) {
			return true;
		}
	}

	return false;
}
