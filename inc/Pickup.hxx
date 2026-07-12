#pragma once

// Floating reagent pickup collected by walking near it.

#include "Object.hxx"
#include "Reagent.hxx"

class Pickup : public Object {
public:
	Pickup() = default;
	Pickup(Vector3 startPosition, ReagentType reagentType, int amount = 1);

	void update(float dt) override;
	void draw(const Camera3D& camera) const override;

	ReagentType reagent{ReagentType::GraveSalt};
	int quantity{1};
	float bobTime{};
};
