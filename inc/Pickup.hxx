#pragma once

#include "Object.hxx"
#include "Reagent.hxx"

class Pickup : public Object {
public:
	Pickup(Vector3 position, ReagentType type, int quantity = 1);

	void update(float dt) override;
	void draw3D() const override;
	ReagentType getType() const { return type; }
	int getQuantity() const { return quantity; }

private:
	ReagentType type;
	int quantity {1};
	float bobTime {};
};
