#pragma once

#include "Object.hxx"
#include "Reagent.hxx"

class Pickup : public Object {
public:
	Pickup(Vector3 start, ReagentType reagent) : reagent(reagent) { position = start; }
	ReagentType getReagent() const { return reagent; }
	void draw() const override {
		const ReagentDefinition &info = getReagentDefinition(reagent);
		DrawSphere(position, 0.16f, info.color);
		DrawSphereWires(position, 0.22f, 6, 6, WHITE);
	}

private:
	ReagentType reagent;
};
