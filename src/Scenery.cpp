#include "Scenery.hxx"

#include <raymath.h>

#include <algorithm>

namespace {
Vector3 BoxCenter(BoundingBox bounds) {
	return Vector3Scale(Vector3Add(bounds.min, bounds.max), 0.5f);
}

Vector3 BoxSize(BoundingBox bounds) {
	return Vector3Subtract(bounds.max, bounds.min);
}
}

Scenery::Scenery(BoundingBox bounds, SceneryStyle style, bool collidable)
	: Object(BoxCenter(bounds), 0.5f), bounds_(bounds), style_(style) {
	setCollisionEnabled(collidable);
}

BoundingBox Scenery::bounds() const {
	return bounds_;
}

SceneryStyle Scenery::style() const {
	return style_;
}

void Scenery::onDraw(const Camera3D&) const {
	const Vector3 center = BoxCenter(bounds_);
	const Vector3 size = BoxSize(bounds_);
	const Color ink {94, 218, 221, 255};
	const Color dimInk {52, 111, 125, 255};
	const Color warmInk {255, 176, 74, 255};
	const Color fill {9, 15, 21, 255};

	switch (style_) {
	case SceneryStyle::Building: {
		DrawCubeV(center, size, fill);
		DrawCubeWiresV(center, size, dimInk);
		const float roofY = bounds_.max.y + size.x * 0.22f;
		DrawLine3D({bounds_.min.x, bounds_.max.y, bounds_.min.z}, {center.x, roofY, bounds_.min.z}, ink);
		DrawLine3D({center.x, roofY, bounds_.min.z}, {bounds_.max.x, bounds_.max.y, bounds_.min.z}, ink);
		DrawLine3D({bounds_.min.x, bounds_.max.y, bounds_.max.z}, {center.x, roofY, bounds_.max.z}, ink);
		DrawLine3D({center.x, roofY, bounds_.max.z}, {bounds_.max.x, bounds_.max.y, bounds_.max.z}, ink);
		DrawLine3D({center.x, roofY, bounds_.min.z}, {center.x, roofY, bounds_.max.z}, ink);
		const float doorX = center.x;
		DrawLine3D({doorX - 0.55f, bounds_.min.y, bounds_.min.z - 0.01f}, {doorX - 0.55f, bounds_.min.y + 1.7f, bounds_.min.z - 0.01f}, warmInk);
		DrawLine3D({doorX + 0.55f, bounds_.min.y, bounds_.min.z - 0.01f}, {doorX + 0.55f, bounds_.min.y + 1.7f, bounds_.min.z - 0.01f}, warmInk);
		DrawLine3D({doorX - 0.55f, bounds_.min.y + 1.7f, bounds_.min.z - 0.01f}, {doorX + 0.55f, bounds_.min.y + 1.7f, bounds_.min.z - 0.01f}, warmInk);
		break;
	}
	case SceneryStyle::Crate:
		DrawCubeV(center, size, {14, 18, 22, 255});
		DrawCubeWiresV(center, size, warmInk);
		DrawLine3D(bounds_.min, bounds_.max, dimInk);
		DrawLine3D({bounds_.max.x, bounds_.min.y, bounds_.min.z}, {bounds_.min.x, bounds_.max.y, bounds_.max.z}, dimInk);
		break;
	case SceneryStyle::Fence: {
		DrawCubeWiresV(center, size, dimInk);
		const int posts = std::max(2, static_cast<int>(std::max(size.x, size.z) / 3.0f));
		for (int i = 0; i <= posts; ++i) {
			const float t = static_cast<float>(i) / static_cast<float>(posts);
			const Vector3 post {
				bounds_.min.x + size.x * t,
				bounds_.min.y,
				bounds_.min.z + size.z * t
			};
			DrawLine3D(post, {post.x, bounds_.max.y + 0.35f, post.z}, i % 2 == 0 ? ink : dimInk);
		}
		break;
	}
	case SceneryStyle::Gallows: {
		const Vector3 left {bounds_.min.x, bounds_.min.y, center.z};
		const Vector3 right {bounds_.max.x, bounds_.min.y, center.z};
		DrawLine3D(left, {left.x, bounds_.max.y, left.z}, warmInk);
		DrawLine3D(right, {right.x, bounds_.max.y, right.z}, warmInk);
		DrawLine3D({left.x, bounds_.max.y, left.z}, {right.x, bounds_.max.y, right.z}, warmInk);
		DrawLine3D({center.x, bounds_.max.y, center.z}, {center.x, bounds_.max.y - 1.15f, center.z}, {220, 220, 190, 255});
		DrawSphereWires({center.x, bounds_.max.y - 1.35f, center.z}, 0.18f, 4, 7, {220, 220, 190, 255});
		break;
	}
	case SceneryStyle::Grave:
		DrawCubeV(center, size, fill);
		DrawCubeWiresV(center, size, {153, 164, 171, 255});
		DrawLine3D({center.x, bounds_.min.y + 0.2f, bounds_.min.z - 0.01f}, {center.x, bounds_.max.y - 0.15f, bounds_.min.z - 0.01f}, {153, 164, 171, 255});
		DrawLine3D({center.x - 0.22f, bounds_.max.y - 0.45f, bounds_.min.z - 0.01f}, {center.x + 0.22f, bounds_.max.y - 0.45f, bounds_.min.z - 0.01f}, {153, 164, 171, 255});
		break;
	case SceneryStyle::Tree: {
		const Vector3 root {center.x, bounds_.min.y, center.z};
		const Vector3 trunk {center.x, bounds_.max.y * 0.62f, center.z};
		DrawLine3D(root, trunk, {157, 132, 104, 255});
		DrawLine3D(trunk, {bounds_.min.x, bounds_.max.y, bounds_.min.z}, dimInk);
		DrawLine3D(trunk, {bounds_.max.x, bounds_.max.y * 0.92f, bounds_.max.z}, dimInk);
		DrawLine3D({center.x, bounds_.max.y * 0.45f, center.z}, {bounds_.min.x, bounds_.max.y * 0.72f, bounds_.max.z}, dimInk);
		DrawLine3D({center.x, bounds_.max.y * 0.45f, center.z}, {bounds_.max.x, bounds_.max.y * 0.68f, bounds_.min.z}, dimInk);
		break;
	}
	}
}
