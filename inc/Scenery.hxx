#pragma once

#include "Object.hxx"
#include "Sprite.hxx"

enum class SceneryVisual {
	Box,
	Fence,
	Cottage
};

class Scenery : public Object {
public:
	Scenery();
	Scenery(Vector3 position, Vector3 size, SceneryVisual visual, bool collidable);

	void setSprite(Sprite* sprite);
	void update(float dt) override;
	void draw3D(const Camera3D& camera) const override;

	BoundingBox getBounds() const;
	Vector3 getSize() const;
private:
	Vector3 size {1.0f, 1.0f, 1.0f};
	SceneryVisual visual {SceneryVisual::Box};
	Sprite* sprite {nullptr};
	float bobPhase {0.0f};
};
