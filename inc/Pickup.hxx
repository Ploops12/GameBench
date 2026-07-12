#pragma once

#include "Reagent.hxx"

class Pickup : public Reagent {
public:
	Pickup(Vector3 start, ReagentType type, int amount = 1);
	void Update(float dt) override;
	void DrawWorld() const;

private:
	float bobTime{0.0f};
};
