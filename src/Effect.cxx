#include "Effect.hxx"

#include <cmath>

void CurseEffect::draw() const {
	if (!active) return;
	const float pulse = 1.0f + sinf(static_cast<float>(GetTime()) * 8.0f) * 0.12f;
	const Color color = kind == EffectKind::Mine ? Color{100, 240, 130, 255} : Color{255, 95, 130, 255};
	if (kind == EffectKind::Mine) {
		DrawCircle3D(position, (0.28f + stats.radius * 0.25f) * pulse, Vector3{0, 1, 0}, 0.0f, color);
		DrawCircle3D(position, (0.5f + stats.radius * 0.25f) * pulse, Vector3{0, 1, 0}, 0.0f, Fade(color, 0.45f));
	} else {
		DrawSphere(position, 0.13f * pulse, color);
		DrawSphereWires(position, 0.18f * pulse, 6, 6, WHITE);
	}
}
