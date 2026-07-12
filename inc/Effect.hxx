#pragma once

#include "CurseVector.hxx"
#include "Object.hxx"

enum class EffectDelivery {
	Projectile,
	Mine,
	Hazard
};

class Effect : public Object {
public:
	Effect(EffectDelivery delivery, Vector3 position, Vector3 velocity, SpellStats stats);

	void update(float dt) override;
	void draw3D() const override;
	EffectDelivery getDelivery() const { return delivery; }
	const SpellStats& getStats() const { return stats; }
	Vector3 getVelocity() const { return velocity; }
	void setVelocity(Vector3 value) { velocity = value; }
	float getAge() const { return age; }
	float getLifetime() const { return lifetime; }
	void setLifetime(float value) { lifetime = value; }
	float getTickTimer() const { return tickTimer; }
	void resetTickTimer(float value) { tickTimer = value; }
	void countTick(float dt) { tickTimer -= dt; }

private:
	EffectDelivery delivery;
	Vector3 velocity {};
	SpellStats stats {};
	float age {};
	float lifetime {8.0f};
	float tickTimer {};
};
