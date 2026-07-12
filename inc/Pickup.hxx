#pragma once

#include "Object.hxx"
#include "Reagent.hxx"

class Pickup : public Object {
public:
	Pickup(ReagentType type, Vector3 position);

	void update(float deltaTime) override;
	void draw() const override;
	bool canCollect(Vector3 playerPosition, float playerRadius) const;
	ReagentType type() const;

private:
	ReagentType reagent{ReagentType::GraveSalt};
	float animationTime{0.0f};
};
