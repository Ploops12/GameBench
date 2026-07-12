#pragma once

#include "Object.hxx"
#include "Reagent.hxx"

class Pickup : public Object {
	// Floating stack collected by proximity.
public:
	Pickup(ReagentType type, int amount, Vector3 position, const Texture2D* texture = nullptr);

	void update(float deltaTime) override;
	void draw3D(const Camera3D& camera) const override;
	bool canCollect(Vector3 playerPosition, float radius = 1.0f) const;
	int takeAmount(int requestedAmount);
	ReagentType getReagentType() const;
	int getAmount() const;

private:
	Reagent reagent;
	Vector3 anchor;
	float age {0.0f};
	const Texture2D* texture {nullptr};
};
