#pragma once

#include "Object.hxx"
#include "Reagent.hxx"

// A reagent stack lying in the world. Collection is deliberately separate
// from update so the owning game can add the stack to its inventory exactly
// once after collectIfNearby succeeds.
class Pickup : public Object {
public:
	Pickup(Vector3 position, ReagentType type, int amount = 1);

	void update(float deltaTime) override;
	void draw(const Camera3D& camera) const override;

	ReagentType getType() const;
	int getAmount() const;
	Vector3 getVisualPosition() const;
	bool collectIfNearby(Vector3 collectorPosition, float collectionRadius);

private:
	ReagentType type;
	int amount {1};
	float bobTime {0.0f};
	float spin {0.0f};
};
