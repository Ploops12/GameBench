#include "Effect.hxx"
#include <cmath>
#include <raymath.h>

Effect::Effect(EffectDelivery delivery, Vector3 position, Vector3 velocity, SpellStats stats) :
	Object(position, 0.24f), delivery(delivery), velocity(velocity), stats(stats) {
	if (delivery == EffectDelivery::Mine) lifetime = 24.0f;
	if (delivery == EffectDelivery::Hazard) lifetime = stats.lingeringDuration;
}

void Effect::update(float dt) {
	age += dt;
	if (delivery == EffectDelivery::Projectile) {
		velocity.y -= 5.2f * dt;
		position = Vector3Add(position, Vector3Scale(velocity, dt));
	}
	if (age >= lifetime) active = false;
}

void Effect::draw3D() const {
	const float pulse = 1.0f + std::sin(age * 12.0f) * 0.12f;
	if (delivery == EffectDelivery::Projectile) {
		DrawSphereWires(position, 0.24f * pulse, 6, 8, Color{225, 85, 105, 255});
		DrawLine3D(position, Vector3Subtract(position, Vector3Scale(velocity, 0.04f)), Color{225, 85, 105, 255});
	} else if (delivery == EffectDelivery::Mine) {
		const Color c = age > 0.6f ? Color{210, 80, 110, 255} : GRAY;
		DrawCircle3D(position, 0.55f * pulse, Vector3{1, 0, 0}, 90.0f, c);
		for (int i = 0; i < 8; ++i) {
			const float a = static_cast<float>(i) * 0.785398f;
			DrawLine3D(position, Vector3{position.x + std::cos(a) * 0.7f, position.y, position.z + std::sin(a) * 0.7f}, c);
		}
	} else {
		const float radius = stats.splashRadius * (0.8f + 0.2f * pulse);
		DrawCircle3D(position, radius, Vector3{1, 0, 0}, 90.0f, Fade(Color{110, 210, 105, 255}, 0.8f));
		DrawCircle3D(Vector3{position.x, position.y + 0.03f, position.z}, radius * 0.65f,
			Vector3{1, 0, 0}, 90.0f, Fade(Color{220, 70, 90, 255}, 0.65f));
	}
}
