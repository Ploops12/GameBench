#include "Effect.hxx"

#include <algorithm>
#include <cmath>

#include <raymath.h>

Effect::Effect(EffectKind initialKind, Vector3 initialPosition, const SpellStats& initialSpell,
	float initialLifetime, float initialRadius)
	: Object(initialPosition, initialRadius), kind(initialKind), spell(initialSpell), lifetime(initialLifetime), radius(initialRadius) {
}

Effect Effect::createProjectile(Vector3 initialPosition, Vector3 initialVelocity, const SpellStats& spell) {
	Effect effect(EffectKind::Projectile, initialPosition, spell, 4.5f, 0.24f);
	effect.velocity = initialVelocity;
	return effect;
}

Effect Effect::createMine(Vector3 initialPosition, const SpellStats& spell) {
	return Effect(EffectKind::Mine, initialPosition, spell, std::max(2.0f, spell.mineDuration), spell.splashRadius);
}

Effect Effect::createBurst(Vector3 initialPosition, const SpellStats& spell, float radiusOverride) {
	Effect effect(EffectKind::Burst, initialPosition, spell, 0.22f,
		radiusOverride > 0.0f ? radiusOverride : std::max(0.35f, spell.splashRadius));
	effect.pulseReady = true;
	return effect;
}

Effect Effect::createHazard(Vector3 initialPosition, const SpellStats& spell) {
	Effect effect(EffectKind::Hazard, initialPosition, spell, std::max(1.0f, spell.hazardDuration),
		std::max(0.8f, spell.splashRadius * 0.72f));
	effect.pulseReady = true;
	effect.pulseTimer = 0.45f;
	return effect;
}

Effect Effect::createTracer(Vector3 start, Vector3 end, Color color) {
	SpellStats tracerSpell {};
	tracerSpell.color = color;
	Effect effect(EffectKind::Tracer, start, tracerSpell, 0.09f, 0.0f);
	effect.tracerEnd = end;
	return effect;
}

void Effect::update(float deltaTime) {
	if (!active) {
		return;
	}

	age += deltaTime;
	if (kind == EffectKind::Projectile) {
		velocity.y -= 13.0f * deltaTime;
		position = Vector3Add(position, Vector3Scale(velocity, deltaTime));
	}

	if (kind == EffectKind::Hazard) {
		pulseTimer -= deltaTime;
		if (pulseTimer <= 0.0f) {
			pulseReady = true;
			pulseTimer += 0.45f;
		}
	}

	if (age >= lifetime) {
		active = false;
	}
}

void Effect::draw3D() const {
	if (!active || !visible) {
		return;
	}

	const float lifeRatio = lifetime > 0.0f ? std::clamp(1.0f - age / lifetime, 0.0f, 1.0f) : 0.0f;
	const Color bright = Fade(spell.color, 0.45f + 0.55f * lifeRatio);
	const Color faint = Fade(spell.color, 0.25f + 0.35f * lifeRatio);

	switch (kind) {
	case EffectKind::Projectile: {
		DrawSphere(position, 0.16f, bright);
		DrawSphereWires(position, 0.22f, 6, 4, RAYWHITE);
		const float speed = Vector3Length(velocity);
		if (speed > 0.01f) {
			const Vector3 tail = Vector3Subtract(position, Vector3Scale(Vector3Normalize(velocity), 0.7f));
			DrawLine3D(tail, position, faint);
		}
		break;
	}
	case EffectKind::Mine:
		DrawCircle3D({position.x, 0.04f, position.z}, radius, {1.0f, 0.0f, 0.0f}, 90.0f, bright);
		DrawCircle3D({position.x, 0.05f, position.z}, radius * 0.52f, {1.0f, 0.0f, 0.0f}, 90.0f, RAYWHITE);
		DrawSphere({position.x, 0.15f, position.z}, 0.14f, bright);
		DrawSphereWires({position.x, 0.15f, position.z}, 0.22f, 6, 4, RAYWHITE);
		break;
	case EffectKind::Burst:
		DrawSphereWires(position, radius * (0.55f + (1.0f - lifeRatio) * 0.65f), 10, 8, bright);
		DrawCircle3D({position.x, 0.05f, position.z}, radius, {1.0f, 0.0f, 0.0f}, 90.0f, faint);
		break;
	case EffectKind::Hazard:
		DrawCircle3D({position.x, 0.035f, position.z}, radius, {1.0f, 0.0f, 0.0f}, 90.0f, bright);
		DrawCircle3D({position.x, 0.05f, position.z}, radius * 0.55f, {1.0f, 0.0f, 0.0f}, 90.0f, faint);
		for (int line = 0; line < 4; ++line) {
			const float angle = (static_cast<float>(line) * 1.5708f) + age * 1.8f;
			const Vector3 start {position.x + std::cos(angle) * radius * 0.25f, 0.03f,
				position.z + std::sin(angle) * radius * 0.25f};
			const Vector3 end {position.x + std::cos(angle) * radius * 0.9f, 0.05f,
				position.z + std::sin(angle) * radius * 0.9f};
			DrawLine3D(start, end, faint);
		}
		break;
	case EffectKind::Tracer:
		DrawLine3D(position, tracerEnd, bright);
		break;
	}
}

bool Effect::isActive() const {
	return active;
}

EffectKind Effect::getKind() const {
	return kind;
}

const SpellStats& Effect::getSpell() const {
	return spell;
}

const Vector3& Effect::getVelocity() const {
	return velocity;
}

void Effect::setVelocity(Vector3 newVelocity) {
	velocity = newVelocity;
}

float Effect::getRadius() const {
	return radius;
}

float Effect::getAge() const {
	return age;
}

float Effect::getLifetime() const {
	return lifetime;
}

bool Effect::isInRadius(Vector3 point, float pointRadius) const {
	return Vector3Distance(position, point) <= radius + pointRadius;
}

bool Effect::consumePulse() {
	if (!pulseReady) {
		return false;
	}
	pulseReady = false;
	return true;
}

void Effect::detonate() {
	if (!active) {
		return;
	}

	kind = EffectKind::Burst;
	age = 0.0f;
	lifetime = 0.22f;
	velocity = {};
	radius = std::max(0.35f, spell.splashRadius);
	pulseReady = true;
}

void Effect::deactivate() {
	active = false;
}
