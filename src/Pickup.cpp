#include "Pickup.hxx"

#include <algorithm>
#include <cmath>

namespace {

constexpr float TWO_PI = 6.28318530718f;
constexpr int RING_SEGMENTS = 20;

Color scaledAlpha(Color color, float scale) {
	color.a = static_cast<unsigned char>(static_cast<float>(color.a) *
		std::clamp(scale, 0.0f, 1.0f));
	return color;
}

void drawRing(Vector3 centre, float radius, Color color) {
	Vector3 previous {centre.x + radius, centre.y, centre.z};
	for (int segment = 1; segment <= RING_SEGMENTS; ++segment) {
		const float angle = TWO_PI * static_cast<float>(segment) /
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

void drawFallbackGlyph(Vector3 centre, ReagentType type, Color color) {
	const float phase = static_cast<float>(static_cast<std::size_t>(type)) * 0.45f;
	DrawSphereWires(centre, 0.17f, 4, 6, color);
	for (int index = 0; index < 4; ++index) {
		const float angle = phase + static_cast<float>(index) * TWO_PI / 4.0f;
		const Vector3 inner {
			centre.x + std::cos(angle) * 0.18f,
			centre.y,
			centre.z + std::sin(angle) * 0.18f
		};
		const Vector3 outer {
			centre.x + std::cos(angle) * 0.29f,
			centre.y + ((index % 2 == 0) ? 0.06f : -0.06f),
			centre.z + std::sin(angle) * 0.29f
		};
		DrawLine3D(inner, outer, color);
	}
}

} // namespace

Pickup::Pickup(ReagentType type, int amount, Vector3 position, const Texture2D* texture)
	: Object(position, {0.58f, 0.7f, 0.58f}),
	  reagent(type, amount),
	  anchor(position),
	  texture(texture) {
	setCollidable(false);
}

void Pickup::update(float deltaTime) {
	if (!isActive() || deltaTime <= 0.0f) return;
	Object::update(deltaTime);
	age += deltaTime;
	position = anchor;
	position.y += 0.12f + std::sin(age * 2.7f) * 0.11f;
}

void Pickup::draw3D(const Camera3D& camera) const {
	if (!isVisible() || !isActive() || reagent.getAmount() <= 0) return;

	const Color reagentColor = getReagentDefinition(reagent.getType()).color;
	const float glow = 0.68f + 0.22f * std::sin(age * 4.1f);
	const Vector3 ringCentre {anchor.x, anchor.y - size.y * 0.38f + 0.025f, anchor.z};
	drawRing(ringCentre, 0.29f + 0.035f * std::sin(age * 2.7f),
		scaledAlpha(reagentColor, glow));
	drawRing(ringCentre, 0.16f, scaledAlpha(reagentColor, 0.42f));

	if (texture != nullptr && texture->id != 0 && texture->width > 0 && texture->height > 0) {
		const Rectangle source {
			0.0f,
			0.0f,
			static_cast<float>(texture->width),
			static_cast<float>(texture->height)
		};
		const float aspect = std::clamp(static_cast<float>(texture->width) /
			static_cast<float>(texture->height), 0.55f, 1.4f);
		DrawBillboardRec(camera, *texture, source, position,
			{0.62f * aspect, 0.62f}, WHITE);
	} else {
		drawFallbackGlyph(position, reagent.getType(), reagentColor);
	}

	const Vector3 top {position.x, position.y + 0.39f, position.z};
	DrawLine3D({top.x, top.y - 0.07f, top.z}, {top.x, top.y + 0.07f, top.z},
		scaledAlpha(reagentColor, glow));
}

bool Pickup::canCollect(Vector3 playerPosition, float radius) const {
	if (!isActive() || reagent.getAmount() <= 0 || radius < 0.0f) return false;
	return CheckCollisionSpheres(playerPosition, radius, position, 0.32f);
}

int Pickup::takeAmount(int requestedAmount) {
	if (!isActive()) return 0;
	const int taken = reagent.take(requestedAmount);
	if (reagent.getAmount() <= 0) {
		setActive(false);
	}
	return taken;
}

ReagentType Pickup::getReagentType() const {
	return reagent.getType();
}

int Pickup::getAmount() const {
	return reagent.getAmount();
}
