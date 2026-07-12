#include "Effect.hxx"

#include <raymath.h>

#include <algorithm>
#include <cmath>

namespace {
Color PayloadColor(const SpellStats& payload) {
	if (payload.wardGain > 0.0f) return {96, 228, 255, 255};
	if (payload.poisonDps > 0.0f) return {179, 255, 68, 255};
	if (payload.fearDuration > 0.0f) return {255, 108, 220, 255};
	if (payload.snareStrength > 0.0f) return {119, 255, 111, 255};
	return {255, 91, 105, 255};
}
}

Effect::Effect(Vector3 position, float radius, float lifetime, SpellStats payload)
	: Object(position, radius), payload_(payload), lifetime_(lifetime) {
	setCollisionEnabled(false);
}

void Effect::update(float dt) {
	age_ += dt;
	if (age_ >= lifetime_) {
		setActive(false);
	}
}

bool Effect::expired() const {
	return !isActive() || age_ >= lifetime_;
}

float Effect::age() const {
	return age_;
}

float Effect::lifetime() const {
	return lifetime_;
}

const SpellStats& Effect::payload() const {
	return payload_;
}

bool Effect::contains(Vector3 point, float targetRadius) const {
	return Vector3Distance(position_, point) <= collisionRadius_ + targetRadius;
}

ProjectileEffect::ProjectileEffect(Vector3 position, Vector3 velocity, SpellStats payload)
	: Effect(position, 0.16f, 5.0f, payload), velocity_(velocity), previousPosition_(position) {
}

void ProjectileEffect::updateProjectile(float dt, Vector3 homingTarget, bool hasTarget) {
	previousPosition_ = position_;
	if (hasTarget && payload_.homingStrength > 0.0f) {
		const float speed = std::max(1.0f, Vector3Length(velocity_));
		Vector3 desired = Vector3Subtract(homingTarget, position_);
		if (Vector3LengthSqr(desired) > 0.001f) {
			desired = Vector3Scale(Vector3Normalize(desired), speed);
			const float turn = std::clamp(payload_.homingStrength * dt, 0.0f, 0.8f);
			velocity_ = Vector3Lerp(velocity_, desired, turn);
		}
	}
	velocity_.y -= 5.5f * dt;
	position_ = Vector3Add(position_, Vector3Scale(velocity_, dt));
	Effect::update(dt);
}

Vector3 ProjectileEffect::velocity() const {
	return velocity_;
}

Vector3 ProjectileEffect::previousPosition() const {
	return previousPosition_;
}

void ProjectileEffect::onDraw(const Camera3D&) const {
	const Color color = PayloadColor(payload_);
	DrawLine3D(previousPosition_, position_, ColorAlpha(color, 0.55f));
	DrawSphere(position_, 0.11f, {7, 10, 14, 255});
	DrawSphereWires(position_, 0.16f, 5, 7, color);
}

TrapEffect::TrapEffect(Vector3 position, SpellStats payload)
	: Effect(position, std::max(1.15f, payload.radius), 18.0f, payload) {
}

void TrapEffect::update(float dt) {
	Effect::update(dt);
}

void TrapEffect::onDraw(const Camera3D&) const {
	const Color color = PayloadColor(payload_);
	const float pulse = 0.20f + std::sin(age_ * 5.0f) * 0.03f;
	DrawSphereWires({position_.x, position_.y + 0.10f, position_.z}, pulse, 4, 7, color);
	for (int i = 0; i < 8; ++i) {
		const float angle = static_cast<float>(i) * PI / 4.0f;
		const Vector3 knee {position_.x + std::cos(angle) * 0.32f, position_.y + 0.08f, position_.z + std::sin(angle) * 0.32f};
		const Vector3 foot {position_.x + std::cos(angle) * 0.48f, position_.y + 0.02f, position_.z + std::sin(angle) * 0.48f};
		DrawLine3D({position_.x, position_.y + 0.10f, position_.z}, knee, color);
		DrawLine3D(knee, foot, color);
	}
	DrawCircle3D({position_.x, position_.y + 0.025f, position_.z}, collisionRadius_, {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(color, 0.25f));
}

HexFieldEffect::HexFieldEffect(Vector3 position, SpellStats payload, float lifetime, bool followsPlayer)
	: Effect(position, std::max(0.75f, payload.radius), std::max(0.6f, lifetime), payload), followsPlayer_(followsPlayer) {
	pulseAccumulator_ = 0.5f;
}

void HexFieldEffect::updateField(float dt, Vector3 playerPosition) {
	if (followsPlayer_) {
		position_ = {playerPosition.x, playerPosition.y + 0.03f, playerPosition.z};
	}
	pulseAccumulator_ += dt;
	Effect::update(dt);
}

bool HexFieldEffect::takePulse() {
	if (pulseAccumulator_ < 0.5f) {
		return false;
	}
	pulseAccumulator_ -= 0.5f;
	return true;
}

bool HexFieldEffect::followsPlayer() const {
	return followsPlayer_;
}

void HexFieldEffect::onDraw(const Camera3D&) const {
	const Color color = PayloadColor(payload_);
	const float phase = std::fmod(age_ * 0.9f, 1.0f);
	DrawCircle3D({position_.x, position_.y + 0.025f, position_.z}, collisionRadius_ * (0.30f + phase * 0.70f),
		{1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(color, 0.75f - phase * 0.55f));
	DrawCircle3D({position_.x, position_.y + 0.03f, position_.z}, collisionRadius_,
		{1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(color, 0.32f));
}

BurstEffect::BurstEffect(Vector3 position, float radius, Color color)
	: Effect(position, radius, 0.38f), color_(color) {
}

void BurstEffect::onDraw(const Camera3D&) const {
	const float progress = std::clamp(age_ / lifetime_, 0.0f, 1.0f);
	const float radius = collisionRadius_ * (0.15f + progress * 0.85f);
	DrawSphereWires(position_, radius, 7, 10, ColorAlpha(color_, 1.0f - progress));
	DrawCircle3D(position_, radius * 0.75f, {1.0f, 0.0f, 0.0f}, 90.0f, ColorAlpha(color_, 0.8f - progress * 0.8f));
}

TraceEffect::TraceEffect(Vector3 start, Vector3 end, Color color)
	: Effect(start, 0.0f, 0.11f), end_(end), color_(color) {
}

void TraceEffect::onDraw(const Camera3D&) const {
	const float alpha = 1.0f - std::clamp(age_ / lifetime_, 0.0f, 1.0f);
	DrawLine3D(position_, end_, ColorAlpha(color_, alpha));
}
