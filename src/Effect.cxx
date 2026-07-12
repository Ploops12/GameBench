#include "Effect.hxx"

#include <algorithm>

Effect::Effect(EffectType typeValue, Vector3 positionValue, float radiusValue, float lifetimeValue)
	: type(typeValue), position(positionValue), radius(radiusValue), lifetime(lifetimeValue), totalLifetime(lifetimeValue) {}

void Effect::Update(float dt) {
	lifetime -= dt;
}

void Effect::Draw() const {
	const float progress = totalLifetime > 0.0f ? std::max(0.0f, lifetime / totalLifetime) : 0.0f;
	Color color = type == EffectType::WardPulse ? SKYBLUE : (type == EffectType::Hazard ? LIME : ORANGE);
	color.a = static_cast<unsigned char>(80.0f + progress * 150.0f);
	DrawSphereWires(position, radius * (1.0f + (1.0f - progress) * 0.3f), 12, 8, color);
	if (type == EffectType::Hazard) {
		DrawCylinder(position, radius, radius, 0.04f, 16, Fade(color, 0.25f));
	}
}
