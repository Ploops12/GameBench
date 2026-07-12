#pragma once

#include "Object.hxx"
#include "Reagent.hxx"

class Pickup : public Object {
public:
	Pickup(Vector3 position, ReagentType type);

	void update(float deltaTime) override;
	void draw3D() const override;
	ReagentType getType() const;
	bool tryCollect(Vector3 playerPosition, float collectionRadius = 1.1f);
	bool isCollected() const;

private:
	ReagentType type {ReagentType::GraveSalt};
	float bobTime {0.0f};
	float baseHeight {0.0f};
	bool collected {false};
};
