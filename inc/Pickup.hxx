#pragma once

#include "Object.hxx"
#include "Reagent.hxx"

class Pickup : public Object {
public:
	Pickup();
	Pickup(Vector3 position, ReagentType type, int amount);

	void update(float dt) override;
	void draw3D(const Camera3D& camera) const override;

	ReagentType getType() const;
	int getAmount() const;
	bool tryCollect(const Vector3& collectorPosition, float radius);
private:
	Reagent reagent {};
	int amount {1};
	float bobTime {0.0f};
};
