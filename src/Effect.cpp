#include "Effect.hxx"

#include <algorithm>
#include <cmath>

#include <raymath.h>

namespace {

constexpr float TWO_PI = 6.28318530718f;
constexpr int RING_SEGMENTS = 28;
constexpr float PULSE_INTERVAL = 0.6f;

Color scaledAlpha(Color color, float scale) {
	color.a = static_cast<unsigned char>(static_cast<float>(color.a) *
		std::clamp(scale, 0.0f, 1.0f));
	return color;
}

void drawRing(Vector3 centre, float radius, Color color, float angularOffset = 0.0f) {
	Vector3 previous {
		centre.x + std::cos(angularOffset) * radius,
		centre.y,
		centre.z + std::sin(angularOffset) * radius
	};
	for (int segment = 1; segment <= RING_SEGMENTS; ++segment) {
		const float angle = angularOffset + TWO_PI * static_cast<float>(segment) /
			static_cast<float>(RING_SEGMENTS);
		const Vector3 next {
			centre.x + std::cos(angle) * radius,
			centre.y,
			centre.z + std::sin(angle) * radius
		};
		DrawLine3D(previous, next, color);
		previous = next;
	}
}

void drawRadialLines(Vector3 centre, float innerRadius, float outerRadius, int count,
	float phase, Color color) {
	for (int index = 0; index < count; ++index) {
		const float angle = phase + TWO_PI * static_cast<float>(index) /
			static_cast<float>(count);
		const Vector3 inner {
			centre.x + std::cos(angle) * innerRadius,
			centre.y,
			centre.z + std::sin(angle) * innerRadius
		};
		const Vector3 outer {
			centre.x + std::cos(angle) * outerRadius,
			centre.y + ((index % 2 == 0) ? 0.035f : 0.0f),
			centre.z + std::sin(angle) * outerRadius
		};
		DrawLine3D(inner, outer, color);
	}
}

void drawSnareSigil(Vector3 centre, float radius, float age, Color color) {
	centre.y += 0.035f;
	drawRing(centre, radius, color, age * 0.15f);
	drawRing(centre, radius * 0.56f, scaledAlpha(color, 0.72f), -age * 0.22f);
	for (int index = 0; index < 6; ++index) {
		const float angle = TWO_PI * static_cast<float>(index) / 6.0f;
		const float nextAngle = angle + TWO_PI / 3.0f;
		const Vector3 start {
			centre.x + std::cos(angle) * radius * 0.82f,
			centre.y,
			centre.z + std::sin(angle) * radius * 0.82f
		};
		const Vector3 end {
			centre.x + std::cos(nextAngle) * radius * 0.82f,
			centre.y,
			centre.z + std::sin(nextAngle) * radius * 0.82f
		};
		DrawLine3D(start, end, scaledAlpha(color, 0.82f));
	}
	drawRadialLines(centre, radius, radius * 1.14f, 12, age * 0.08f, color);
}

} // namespace

Effect::Effect(EffectKind kind, Vector3 position, float radius, float duration, Color color,
	const SpellStats& spellStats)
	: Object(position, {
		std::max(0.02f, radius) * 2.0f,
		std::max(0.12f, radius * 0.4f),
		std::max(0.02f, radius) * 2.0f
	}),
	  kind(kind),
	  radius(std::max(0.02f, radius)),
	  duration(std::max(0.01f, duration)),
	  remainingTime(std::max(0.01f, duration)),
	  color(color),
	  spellStats(spellStats) {
	setCollidable(false);
}

void Effect::update(float deltaTime) {
	if (!isActive() || deltaTime <= 0.0f) return;
	Object::update(deltaTime);
	age += deltaTime;
	remainingTime = std::max(0.0f, remainingTime - deltaTime);

	if (kind == EffectKind::LingeringRot) {
		pulseAccumulator += deltaTime;
		if (pulseAccumulator >= PULSE_INTERVAL) {
			pulseAccumulator = std::fmod(pulseAccumulator, PULSE_INTERVAL);
			pulseReady = true;
		}
	}

	if (remainingTime <= 0.0f) {
		pulseReady = false;
		setActive(false);
	}
}

void Effect::draw3D(const Camera3D& camera) const {
	if (!isVisible() || !isActive()) return;

	const float lifeRatio = std::clamp(remainingTime / duration, 0.0f, 1.0f);
	const float appear = std::clamp(age * 7.0f, 0.0f, 1.0f);
	const float fade = std::min(appear, std::clamp(lifeRatio * 3.0f, 0.0f, 1.0f));
	const Color lineColor = scaledAlpha(color, fade);
	const Vector3 floorCentre {position.x, position.y + 0.035f, position.z};

	switch (kind) {
		case EffectKind::Burst: {
			const float progress = std::clamp(age / duration, 0.0f, 1.0f);
			const float burstRadius = radius * (0.15f + progress * 0.85f);
			const float cameraDistance = Vector3Distance(camera.position, position);
			if (cameraDistance < radius * 1.25f) {
				const Vector3 groundCentre {position.x, 0.04f, position.z};
				drawRing(groundCentre, burstRadius, lineColor, age * 0.35f);
				drawRadialLines(groundCentre, burstRadius * 0.62f, burstRadius * 1.18f, 12,
					age * 1.7f, lineColor);
			} else {
				DrawSphereWires(position, burstRadius, 5, 8, lineColor);
				drawRadialLines(position, burstRadius * 0.62f, burstRadius * 1.18f, 12,
					age * 1.7f, lineColor);
			}
			break;
		}
		case EffectKind::LingeringRot: {
			const float pulse = 0.5f + 0.5f * std::sin(age * TWO_PI / PULSE_INTERVAL);
			drawRing(floorCentre, radius, lineColor, age * 0.12f);
			drawRing(floorCentre, radius * (0.48f + pulse * 0.16f),
				scaledAlpha(lineColor, 0.5f + pulse * 0.35f), -age * 0.18f);
			drawRadialLines(floorCentre, radius * 0.72f, radius * (0.88f + pulse * 0.1f),
				9, age * 0.23f, scaledAlpha(lineColor, 0.7f));
			for (int index = 0; index < 5; ++index) {
				const float angle = age * 0.25f + TWO_PI * static_cast<float>(index) / 5.0f;
				const float wispRadius = radius * (0.22f + 0.12f * static_cast<float>(index % 3));
				const Vector3 base {
					position.x + std::cos(angle) * wispRadius,
					position.y,
					position.z + std::sin(angle) * wispRadius
				};
				DrawLine3D(base, {base.x + std::sin(age + static_cast<float>(index)) * 0.08f,
					base.y + 0.25f + pulse * 0.16f, base.z}, scaledAlpha(lineColor, 0.55f));
			}
			break;
		}
		case EffectKind::SnareSigil:
			drawSnareSigil(floorCentre, radius, age, lineColor);
			break;
		case EffectKind::FearPulse: {
			const float progress = std::clamp(age / duration, 0.0f, 1.0f);
			drawRing(floorCentre, radius * (0.1f + 0.9f * progress), lineColor,
				age * 0.4f);
			drawRing(floorCentre, radius * (0.05f + 0.62f * progress),
				scaledAlpha(lineColor, 0.55f), -age * 0.5f);
			drawRadialLines(floorCentre, radius * progress * 0.72f,
				radius * progress, 10, age * 0.3f, lineColor);
			break;
		}
		case EffectKind::WardPulse: {
			const float progress = std::clamp(age / duration, 0.0f, 1.0f);
			const float wardRadius = radius * (0.4f + 0.6f * progress);
			if (Vector3Distance(camera.position, position) >= wardRadius * 1.25f) {
				DrawSphereWires(position, wardRadius, 6, 10, lineColor);
				DrawLine3D({position.x, position.y - wardRadius, position.z},
					{position.x, position.y + wardRadius, position.z}, scaledAlpha(lineColor, 0.68f));
			}
			drawRing({position.x, 0.04f, position.z}, wardRadius,
				scaledAlpha(lineColor, 0.62f), age * 0.3f);
			break;
		}
		case EffectKind::HitSpark: {
			const float progress = std::clamp(age / duration, 0.0f, 1.0f);
			const float sparkRadius = radius * (0.35f + progress * 0.75f);
			drawRadialLines(position, sparkRadius * 0.18f, sparkRadius, 8,
				age * 2.2f, lineColor);
			DrawSphereWires(position, sparkRadius * 0.22f, 4, 5, lineColor);
			break;
		}
	}
}

bool Effect::contains(Vector3 point) const {
	if (!isActive()) return false;
	const float deltaX = point.x - position.x;
	const float deltaZ = point.z - position.z;
	const float verticalReach = std::max(1.5f, radius);
	return deltaX * deltaX + deltaZ * deltaZ <= radius * radius &&
		std::fabs(point.y - position.y) <= verticalReach;
}

bool Effect::takePulse() {
	if (!isActive() || !pulseReady) return false;
	pulseReady = false;
	return true;
}

EffectKind Effect::getKind() const {
	return kind;
}

const SpellStats& Effect::getSpellStats() const {
	return spellStats;
}

float Effect::getRemainingTime() const {
	return remainingTime;
}
