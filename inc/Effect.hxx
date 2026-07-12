#pragma once

#include "CurseVector.hxx"
#include "Object.hxx"

enum class EffectKind {
	Projectile,
	Mine,
	Hazard,
	Burst
};

class Effect : public Object {
public:
	Effect(EffectKind kind, Vector3 position, Vector3 velocity, const CurseVector& spell, float lifetime);

	void update(float deltaTime) override;
	void draw() const override;

	EffectKind kind() const;
	const CurseVector& spell() const;
	float lifetime() const;
	bool expired() const;
	bool armed() const;
	void arm();
	void setLifetime(float lifetime);
	Vector3 velocity() const;
	void setVelocity(Vector3 newVelocity);
	bool pulseReady(float interval);

private:
	EffectKind effectKind{EffectKind::Projectile};
	CurseVector curse;
	Vector3 travelVelocity{};
	float remainingLife{0.0f};
	float armTimer{0.35f};
	float pulseTimer{0.0f};
};
