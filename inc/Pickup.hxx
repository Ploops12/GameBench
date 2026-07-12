#pragma once

#include "Object.hxx"
#include "Reagent.hxx"

class Pickup : public Object {
public:
	Pickup(Vector3 position, ReagentType type, int quantity = 1);

	void update(float dt) override;
	[[nodiscard]] bool collectIfNear(Vector3 playerPosition, float range);
	void drawBillboardHud(const Camera3D& camera) const;

	[[nodiscard]] ReagentType type() const;
	[[nodiscard]] int quantity() const;

protected:
	void onDraw(const Camera3D& camera) const override;

private:
	ReagentType type_;
	int quantity_ {1};
	float age_ {0.0f};
	float baseHeight_ {0.35f};
};
