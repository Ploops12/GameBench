#pragma once

#include "Object.hxx"

// A compact piece of static world geometry. Positions refer to the middle of
// the footprint on the ground, rather than the centre of the volume.
class Scenery : public Object {
public:
	enum class Style {
		Box,
		Chapel
	};

	Scenery(Vector3 position = {}, Vector3 size = {2.0f, 2.0f, 2.0f},
		bool solid = true, Style style = Style::Box,
		Color lineColor = {142, 211, 191, 255});

	void update(float deltaTime) override;
	void draw(const Camera3D& camera) const override;

	BoundingBox getBounds() const;
	BoundingBox getBoundingBox() const;
	const Vector3& getSize() const;
	void setSize(Vector3 value);
	Style getStyle() const;
	void setStyle(Style value);
	Color getLineColor() const;
	void setLineColor(Color value);
	bool isSolid() const;

private:
	Vector3 size {2.0f, 2.0f, 2.0f};
	Style style {Style::Box};
	Color lineColor {142, 211, 191, 255};
};
