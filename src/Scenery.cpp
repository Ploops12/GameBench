#include "Scenery.hxx"

#include <algorithm>
#include <cmath>

namespace {

constexpr float MIN_PROP_THICKNESS = 0.06f;

void drawOutlinedBox(Vector3 position, Vector3 size, Color color, float fillAlpha = 0.16f) {
	DrawCubeV(position, size, Fade(color, fillAlpha));
	DrawCubeWiresV(position, size, color);
}

void drawFence(Vector3 position, Vector3 size, Color color) {
	const bool alongX = size.x >= size.z;
	const float length = std::max(alongX ? size.x : size.z, 0.5f);
	const float height = std::max(size.y, 0.5f);
	const float thickness = std::max(MIN_PROP_THICKNESS, std::min(size.x, size.z));
	const int postCount = std::clamp(static_cast<int>(std::ceil(length / 1.8f)) + 1, 2, 10);

	for (int post = 0; post < postCount; ++post) {
		const float offset = -length * 0.5f + length * static_cast<float>(post) /
			static_cast<float>(postCount - 1);
		Vector3 postPosition = position;
		if (alongX) postPosition.x += offset;
		else postPosition.z += offset;
		drawOutlinedBox(postPosition, {thickness, height, thickness}, color, 0.12f);
	}

	for (float verticalOffset : {-height * 0.22f, height * 0.2f}) {
		Vector3 railPosition = position;
		railPosition.y += verticalOffset;
		const Vector3 railSize = alongX
			? Vector3 {length, thickness, thickness}
			: Vector3 {thickness, thickness, length};
		drawOutlinedBox(railPosition, railSize, color, 0.1f);
	}
}

void drawDeadTree(Vector3 position, Vector3 size, Color color) {
	const float height = std::max(size.y, 1.0f);
	const float spread = std::max(std::max(size.x, size.z), 0.8f);
	const Vector3 base {position.x, position.y - height * 0.5f, position.z};
	const Vector3 fork {position.x, position.y + height * 0.18f, position.z};
	const Vector3 crown {position.x, position.y + height * 0.5f, position.z};

	DrawLine3D(base, crown, color);
	DrawLine3D({base.x + 0.035f, base.y, base.z}, {crown.x + 0.035f, crown.y, crown.z}, color);
	DrawLine3D(fork, {position.x - spread * 0.48f, position.y + height * 0.42f,
		position.z + spread * 0.12f}, color);
	DrawLine3D(fork, {position.x + spread * 0.45f, position.y + height * 0.36f,
		position.z - spread * 0.18f}, color);
	DrawLine3D({position.x - spread * 0.24f, position.y + height * 0.30f,
		position.z + spread * 0.06f},
		{position.x - spread * 0.38f, position.y + height * 0.17f,
		position.z + spread * 0.3f}, color);
}

void drawGallows(Vector3 position, Vector3 size, Color color) {
	const float width = std::max(size.x, 1.2f);
	const float height = std::max(size.y, 2.0f);
	const float depth = std::max(size.z, 0.12f);
	const float thickness = std::max(MIN_PROP_THICKNESS, std::min(width, height) * 0.045f);
	const float bottom = position.y - height * 0.5f;
	const float top = position.y + height * 0.5f;

	drawOutlinedBox({position.x - width * 0.42f, position.y, position.z},
		{thickness, height, depth}, color, 0.1f);
	drawOutlinedBox({position.x + width * 0.42f, position.y, position.z},
		{thickness, height, depth}, color, 0.1f);
	drawOutlinedBox({position.x, top, position.z}, {width, thickness, depth}, color, 0.1f);
	DrawLine3D({position.x + width * 0.18f, top, position.z},
		{position.x + width * 0.18f, top - height * 0.32f, position.z}, color);
	DrawLine3D({position.x - width * 0.42f, bottom, position.z - depth * 0.7f},
		{position.x - width * 0.42f, bottom, position.z + depth * 0.7f}, color);
	DrawLine3D({position.x + width * 0.42f, bottom, position.z - depth * 0.7f},
		{position.x + width * 0.42f, bottom, position.z + depth * 0.7f}, color);
}

void drawLantern(Vector3 position, Vector3 size, Color color) {
	const float height = std::max(size.y, 1.2f);
	const float thickness = std::max(MIN_PROP_THICKNESS, std::min(size.x, size.z) * 0.22f);
	const float lanternSize = std::max(0.18f, std::max(size.x, size.z) * 0.65f);
	const float top = position.y + height * 0.5f;

	drawOutlinedBox({position.x, position.y - lanternSize * 0.25f, position.z},
		{thickness, height - lanternSize * 0.5f, thickness}, color, 0.1f);
	drawOutlinedBox({position.x, top - lanternSize * 0.45f, position.z},
		{lanternSize, lanternSize * 0.78f, lanternSize}, ORANGE, 0.28f);
	DrawSphereWires({position.x, top - lanternSize * 0.45f, position.z},
		lanternSize * 0.72f, 4, 8, Fade(ORANGE, 0.45f));
}

} // namespace

Scenery::Scenery(SceneryKind sceneryKind, Vector3 worldPosition, Vector3 dimensions, Color tint,
	bool blocksMovement)
	: Object(worldPosition, dimensions), kind(sceneryKind), color(tint) {
	collidable = blocksMovement;
}

void Scenery::draw3D(const Camera3D& camera) const {
	(void)camera;
	if (!visible) return;

	switch (kind) {
	case SceneryKind::SolidBox:
		drawOutlinedBox(position, size, color, 0.24f);
		break;
	case SceneryKind::Fence:
		drawFence(position, size, color);
		break;
	case SceneryKind::DeadTree:
		drawDeadTree(position, size, color);
		break;
	case SceneryKind::Gallows:
		drawGallows(position, size, color);
		break;
	case SceneryKind::Lantern:
		drawLantern(position, size, color);
		break;
	case SceneryKind::StandingStone:
		drawOutlinedBox(position, size, color, 0.12f);
		DrawLine3D({position.x - size.x * 0.26f, position.y, position.z - size.z * 0.51f},
			{position.x, position.y + size.y * 0.25f, position.z - size.z * 0.51f}, color);
		DrawLine3D({position.x, position.y + size.y * 0.25f, position.z - size.z * 0.51f},
			{position.x + size.x * 0.26f, position.y, position.z - size.z * 0.51f}, color);
		break;
	}
}

SceneryKind Scenery::getKind() const {
	return kind;
}

Color Scenery::getColor() const {
	return color;
}
