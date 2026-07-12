#pragma once

#include "CurseVector.hxx"
#include "Object.hxx"

enum class EffectMode {
	Projectile,
	Mine,
	Hazard,
	SelfPulse
};

class Effect : public Object {
public:
	Effect();

	static Effect makeProjectile(const Vector3& start, const Vector3& direction, const ComposedSpell& spell);
	static Effect makeMine(const Vector3& start, const ComposedSpell& spell);
	static Effect makeHazard(const Vector3& start, const ComposedSpell& spell, float durationOverride = 0.0f);
	static Effect makeSelfPulse(const Vector3& start, const ComposedSpell& spell);

	void update(float dt) override;
	void draw3D(const Camera3D& camera) const override;

	EffectMode getMode() const;
	const ComposedSpell& getSpell() const;
	Vector3 getVelocity() const;
	void setVelocity(Vector3 newVelocity);
	float getTimeLeft() const;
	void setTimeLeft(float value);
	float getTickTimer() const;
	void setTickTimer(float value);
	float getTriggerRadius() const;
	bool hasTriggered() const;
	void trigger();

private:
	EffectMode mode;
	ComposedSpell spell;
	Vector3 velocity;
	float timeLeft;
	float tickTimer;
	float triggerRadius;
	bool triggered;
};
