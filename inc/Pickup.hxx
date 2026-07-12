#pragma once

#include "Object.hxx"
#include "Reagent.hxx"

class Pickup : public Object {
public:
	Pickup();
	Pickup(Vector3 startPosition, ReagentType pickupType, int pickupAmount);

	void update(float dt) override;
	void draw3D(const Camera3D& camera) const override;

	ReagentType getType() const;
	int getAmount() const;

private:
	ReagentType type;
	int amount;
	float bobTime;
};
