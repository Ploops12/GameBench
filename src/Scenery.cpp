#include "Scenery.hxx"

#include <algorithm>
#include <cmath>

namespace {
Vector3 sanitizeSize(Vector3 value) {
	return Vector3 {
		std::max(0.1f, std::abs(value.x)),
		std::max(0.1f, std::abs(value.y)),
		std::max(0.1f, std::abs(value.z))
	};
}
}

Scenery::Scenery(Vector3 initialPosition, Vector3 initialSize, bool solid,
	Style initialStyle, Color initialLineColor)
	: Object(initialPosition, 0.0f), size(sanitizeSize(initialSize)), style(initialStyle), lineColor(initialLineColor) {
	setCollisionEnabled(solid);
	collisionRadius = 0.5f * std::sqrt(size.x * size.x + size.z * size.z);
}

void Scenery::update(float) {
}

void Scenery::draw(const Camera3D&) const {
	if (!isActive() || !isDrawable()) {
		return;
	}

	const Vector3 centre {
		position.x,
		position.y + size.y * 0.5f,
		position.z
	};
	DrawCubeWires(centre, size.x, size.y, size.z, lineColor);

	if (style != Style::Chapel) {
		return;
	}

	const float halfWidth = size.x * 0.5f;
	const float halfDepth = size.z * 0.5f;
	const float top = position.y + size.y;
	const float ridge = top + std::max(0.35f, size.y * 0.42f);
	const Vector3 frontLeft {position.x - halfWidth, top, position.z - halfDepth};
	const Vector3 frontRight {position.x + halfWidth, top, position.z - halfDepth};
	const Vector3 backLeft {position.x - halfWidth, top, position.z + halfDepth};
	const Vector3 backRight {position.x + halfWidth, top, position.z + halfDepth};
	const Vector3 frontPeak {position.x, ridge, position.z - halfDepth};
	const Vector3 backPeak {position.x, ridge, position.z + halfDepth};

	DrawLine3D(frontLeft, frontPeak, lineColor);
	DrawLine3D(frontPeak, frontRight, lineColor);
	DrawLine3D(backLeft, backPeak, lineColor);
	DrawLine3D(backPeak, backRight, lineColor);
	DrawLine3D(frontPeak, backPeak, lineColor);

	const float crossZ = position.z - halfDepth - 0.012f;
	const float crossHeight = std::min(size.y * 0.38f, 0.85f);
	const Vector3 crossCentre {position.x, top - size.y * 0.33f, crossZ};
	DrawLine3D(
		Vector3 {crossCentre.x, crossCentre.y - crossHeight * 0.5f, crossCentre.z},
		Vector3 {crossCentre.x, crossCentre.y + crossHeight * 0.5f, crossCentre.z}, lineColor);
	DrawLine3D(
		Vector3 {crossCentre.x - crossHeight * 0.25f, crossCentre.y + crossHeight * 0.1f, crossCentre.z},
		Vector3 {crossCentre.x + crossHeight * 0.25f, crossCentre.y + crossHeight * 0.1f, crossCentre.z}, lineColor);
}

BoundingBox Scenery::getBounds() const {
	const Vector3 halfSize {size.x * 0.5f, 0.0f, size.z * 0.5f};
	return BoundingBox {
		Vector3 {position.x - halfSize.x, position.y, position.z - halfSize.z},
		Vector3 {position.x + halfSize.x, position.y + size.y, position.z + halfSize.z}
	};
}

BoundingBox Scenery::getBoundingBox() const {
	return getBounds();
}

const Vector3& Scenery::getSize() const {
	return size;
}

void Scenery::setSize(Vector3 value) {
	size = sanitizeSize(value);
	collisionRadius = 0.5f * std::sqrt(size.x * size.x + size.z * size.z);
}

Scenery::Style Scenery::getStyle() const {
	return style;
}

void Scenery::setStyle(Style value) {
	style = value;
}

Color Scenery::getLineColor() const {
	return lineColor;
}

void Scenery::setLineColor(Color value) {
	lineColor = value;
}

bool Scenery::isSolid() const {
	return hasCollision();
}
