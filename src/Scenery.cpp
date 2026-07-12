#include "Scenery.hxx"

Scenery::Scenery()
	: Object({}, 0.5f) {
}

Scenery::Scenery(Vector3 position, Vector3 size, SceneryVisual visual, bool collidable)
	: Object(position, size.x * 0.5f), size(size), visual(visual) {
	this->collidable = collidable;
}

void Scenery::setSprite(Sprite* value) {
	sprite = value;
}

void Scenery::update(float dt) {
	bobPhase += dt;
}

void Scenery::draw3D(const Camera3D& camera) const {
	Color tint = Color {28, 31, 40, 255};
	Color wire = Color {180, 210, 230, 180};

	if (visual == SceneryVisual::Box || !sprite || !sprite->isLoaded()) {
		DrawCube(position, size.x, size.y, size.z, tint);
		DrawCubeWires(position, size.x, size.y, size.z, wire);
		return;
	}

	const Texture2D& texture = sprite->getTexture();
	Rectangle source {0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height)};
	Vector2 drawSize {
		visual == SceneryVisual::Fence ? size.x : size.x * 1.15f,
		size.y
	};
	DrawBillboardRec(camera, texture, source, position, drawSize, WHITE);

	if (collidable) {
		DrawCubeWires(position, size.x, size.y, size.z, Color {100, 120, 150, 70});
	}
}

BoundingBox Scenery::getBounds() const {
	return BoundingBox {
		Vector3 {position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z - size.z * 0.5f},
		Vector3 {position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z + size.z * 0.5f}
	};
}

Vector3 Scenery::getSize() const {
	return size;
}
