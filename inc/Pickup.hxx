#pragma once

#include "Object.hxx"
#include "Reagent.hxx"

class Pickup : public Object {
public:
	Pickup(Vector3 position = {}, ReagentType type = ReagentType::GraveSalt);
	void update(float dt) override;
	void draw() const override;
	ReagentType type;
	float age{0.0f};
};
