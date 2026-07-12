#include "Effect.hxx"

#include <raymath.h>

#include <algorithm>

namespace {
	constexpr Color CURSE_WHITE{226, 230, 211, 255};
	constexpr Color CURSE_RED{224, 80, 92, 255};
	constexpr Color CURSE_GREEN{114, 214, 128, 255};
	constexpr Color CURSE_BLUE{102, 206, 221, 255};
}

Effect::Effect(EffectKind type, Vector3 initialPosition, Vector3 initialVelocity, const CurseVector& spellVector, float lifetime)
	: Object(initialPosition), effectKind(type), curse(spellVector), travelVelocity(initialVelocity), remainingLife(lifetime) {
	if (type != EffectKind::Mine) armTimer = 0.0f;
}

void Effect::update(float deltaTime) {
	if (!active) return;
	remainingLife -= deltaTime;
	armTimer = std::max(0.0f, armTimer - deltaTime);
	pulseTimer = std::max(0.0f, pulseTimer - deltaTime);
	if (effectKind == EffectKind::Projectile) position = Vector3Add(position, Vector3Scale(travelVelocity, deltaTime));
	if (remainingLife <= 0.0f) active = false;
}

void Effect::draw() const {
	if (!active) return;
	const SpellStats spellStats = curse.stats();
	const Color accent = spellStats.seeking ? CURSE_BLUE : (spellStats.poisonPerSecond > 0.0f ? CURSE_GREEN : CURSE_RED);

	switch (effectKind) {
		case EffectKind::Projectile: {
			DrawSphere(position, 0.14f + static_cast<float>(spellStats.reagentCount) * 0.035f, accent);
			DrawSphereWires(position, 0.2f + static_cast<float>(spellStats.reagentCount) * 0.035f, 6, 6, CURSE_WHITE);
			const Vector3 trail = Vector3Add(position, Vector3Scale(travelVelocity, -0.12f));
			DrawLine3D(position, trail, ColorAlpha(accent, 0.65f));
			break;
		}
		case EffectKind::Mine: {
			const float mineRadius = std::max(0.45f, spellStats.radius * 0.45f);
			DrawCylinder({position.x, 0.08f, position.z}, mineRadius, mineRadius * 0.7f, 0.13f, 8, ColorAlpha(accent, 0.3f));
			DrawCylinderWires({position.x, 0.08f, position.z}, mineRadius, mineRadius * 0.7f, 0.13f, 8, CURSE_WHITE);
			DrawCircle3D({position.x, 0.02f, position.z}, mineRadius, {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(accent, 0.8f));
			DrawLine3D({position.x - mineRadius, 0.04f, position.z}, {position.x + mineRadius, 0.04f, position.z}, accent);
			DrawLine3D({position.x, 0.04f, position.z - mineRadius}, {position.x, 0.04f, position.z + mineRadius}, accent);
			break;
		}
		case EffectKind::Hazard: {
			const float hazardRadius = std::max(0.5f, spellStats.radius);
			DrawCircle3D({position.x, 0.025f, position.z}, hazardRadius, {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(CURSE_GREEN, 0.35f));
			DrawCircle3D({position.x, 0.03f, position.z}, hazardRadius * 0.7f, {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(CURSE_RED, 0.6f));
			break;
		}
		case EffectKind::Burst:
			DrawSphereWires(position, std::max(0.4f, spellStats.radius), 12, 8, CURSE_WHITE);
			break;
	}
}

EffectKind Effect::kind() const {
	return effectKind;
}

const CurseVector& Effect::spell() const {
	return curse;
}

float Effect::lifetime() const {
	return remainingLife;
}

bool Effect::expired() const {
	return !active || remainingLife <= 0.0f;
}

bool Effect::armed() const {
	return armTimer <= 0.0f;
}

void Effect::arm() {
	armTimer = 0.0f;
}

void Effect::setLifetime(float lifetimeValue) {
	remainingLife = lifetimeValue;
	active = lifetimeValue > 0.0f;
}

Vector3 Effect::velocity() const {
	return travelVelocity;
}

void Effect::setVelocity(Vector3 newVelocity) {
	travelVelocity = newVelocity;
}

bool Effect::pulseReady(float interval) {
	if (pulseTimer > 0.0f) return false;
	pulseTimer = std::max(0.05f, interval);
	return true;
}
