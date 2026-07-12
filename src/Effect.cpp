#include <cmath>
#include "Effect.hxx"
#include <raymath.h>

Effect::Effect()
	: Object(),
		mode(EffectMode::Projectile),
		spell(),
		velocity {0.0f, 0.0f, 0.0f},
		timeLeft(0.0f),
		tickTimer(0.0f),
		triggerRadius(0.0f),
		triggered(false) {}

Effect Effect::makeProjectile(const Vector3& start, const Vector3& direction, const ComposedSpell& spellData) {
	Effect effect;
	effect.mode = EffectMode::Projectile;
	effect.position = start;
	effect.spell = spellData;
	effect.velocity = Vector3Scale(Vector3Normalize(direction), spellData.projectileSpeed);
	effect.collisionRadius = 0.2f;
	effect.timeLeft = spellData.lifetime;
	effect.collidable = false;
	return effect;
}

Effect Effect::makeMine(const Vector3& start, const ComposedSpell& spellData) {
	Effect effect;
	effect.mode = EffectMode::Mine;
	effect.position = start;
	effect.position.y = 0.05f;
	effect.spell = spellData;
	effect.collisionRadius = spellData.splashRadius;
	effect.timeLeft = spellData.lifetime;
	effect.triggerRadius = spellData.triggerRadius;
	return effect;
}

Effect Effect::makeHazard(const Vector3& start, const ComposedSpell& spellData, float durationOverride) {
	Effect effect;
	effect.mode = EffectMode::Hazard;
	effect.position = start;
	effect.position.y = 0.02f;
	effect.spell = spellData;
	effect.collisionRadius = spellData.splashRadius;
	effect.timeLeft = durationOverride > 0.0f ? durationOverride : spellData.hazardDuration;
	effect.tickTimer = 0.2f;
	return effect;
}

Effect Effect::makeSelfPulse(const Vector3& start, const ComposedSpell& spellData) {
	Effect effect;
	effect.mode = EffectMode::SelfPulse;
	effect.position = start;
	effect.spell = spellData;
	effect.collisionRadius = spellData.splashRadius;
	effect.timeLeft = 0.12f;
	effect.triggered = true;
	return effect;
}

void Effect::update(float dt) {
	timeLeft -= dt;
	if (timeLeft <= 0.0f) {
		active = false;
		return;
	}

	switch (mode) {
		case EffectMode::Projectile:
			velocity.y -= 6.0f * dt;
			position = Vector3Add(position, Vector3Scale(velocity, dt));
			break;
		case EffectMode::Mine:
			position.y = 0.08f + std::sin(static_cast<float>(GetTime()) * 6.0f) * 0.03f;
			break;
		case EffectMode::Hazard:
			tickTimer -= dt;
			break;
		case EffectMode::SelfPulse:
			break;
	}
}

void Effect::draw3D(const Camera3D&) const {
	Color baseTint = getBaseVectorColor(spell.base);

	switch (mode) {
		case EffectMode::Projectile:
			DrawSphere(position, 0.16f, baseTint);
			DrawSphereWires(position, 0.18f, 8, 8, WHITE);
			break;
		case EffectMode::Mine:
			DrawSphere(position, 0.22f, Fade(baseTint, 0.65f));
			DrawCircle3D(position, triggerRadius, {1.0f, 0.0f, 0.0f}, 90.0f, WHITE);
			break;
		case EffectMode::Hazard:
			DrawCircle3D(position, collisionRadius, {1.0f, 0.0f, 0.0f}, 90.0f, Fade(baseTint, 0.45f));
			DrawCircle3D(position, collisionRadius * 0.65f, {1.0f, 0.0f, 0.0f}, 90.0f, Fade(WHITE, 0.6f));
			break;
		case EffectMode::SelfPulse:
			DrawCircle3D(position, collisionRadius, {1.0f, 0.0f, 0.0f}, 90.0f, Fade(baseTint, 0.75f));
			break;
	}
}

EffectMode Effect::getMode() const {
	return mode;
}

const ComposedSpell& Effect::getSpell() const {
	return spell;
}

Vector3 Effect::getVelocity() const {
	return velocity;
}

void Effect::setVelocity(Vector3 newVelocity) {
	velocity = newVelocity;
}

float Effect::getTimeLeft() const {
	return timeLeft;
}

void Effect::setTimeLeft(float value) {
	timeLeft = value;
}

float Effect::getTickTimer() const {
	return tickTimer;
}

void Effect::setTickTimer(float value) {
	tickTimer = value;
}

float Effect::getTriggerRadius() const {
	return triggerRadius;
}

bool Effect::hasTriggered() const {
	return triggered;
}

void Effect::trigger() {
	triggered = true;
}
