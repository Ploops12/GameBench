#pragma once

#include "Object.hxx"
#include "Reagent.hxx"

class Pickup : public Object {
public:
	Pickup(Vector3 position, ReagentType type);

	void update(float time);
	void draw3D() const override;
	ReagentType getType() const;

private:
	ReagentType type;
	float bobOffset {};
};
