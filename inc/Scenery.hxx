#pragma once

#include "Object.hxx"

enum class SceneryStyle {
	Building,
	Crate,
	Fence,
	Gallows,
	Grave,
	Tree
};

class Scenery : public Object {
public:
	Scenery(BoundingBox bounds, SceneryStyle style, bool collidable = true);

	[[nodiscard]] BoundingBox bounds() const override;
	[[nodiscard]] SceneryStyle style() const;

protected:
	void onDraw(const Camera3D& camera) const override;

private:
	BoundingBox bounds_ {};
	SceneryStyle style_ {SceneryStyle::Crate};
};
