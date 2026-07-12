#include "Effect.hxx"

#include <algorithm>

Effect::Effect(Vector3 initialPosition, float initialRadius, float duration, const SpellStats& initialSpell)
	: position(initialPosition), radius(initialRadius), timeLeft(duration), totalTime(duration), spell(initialSpell) {}

bool Effect::update(float deltaTime) {
	timeLeft -= deltaTime;
	return timeLeft > 0.0f;
}

bool Effect::isAlive() const {
	return timeLeft > 0.0f;
}

void Effect::draw3D() const {
	float pulse = 0.85f + 0.15f * (timeLeft / std::max(0.01f, totalTime));
	Color color = spell.poisonDuration > 0.0f ? Color {226, 183, 64, 160} : Color {229, 91, 108, 160};
	DrawCircle3D({position.x, 0.035f, position.z}, radius * pulse, {1.0f, 0.0f, 0.0f}, 90.0f, color);
	DrawCircle3D({position.x, 0.04f, position.z}, radius * 0.55f, {1.0f, 0.0f, 0.0f}, 90.0f, Fade(color, 0.55f));
}

Vector3 Effect::getPosition() const { return position; }
float Effect::getRadius() const { return radius; }
const SpellStats& Effect::getSpell() const { return spell; }
