#pragma once

#include "CurseVector.hxx"
#include "Object.hxx"

enum class EffectKind {
	Projectile,
	Mine,
	Burst,
	Hazard,
	Tracer
};

class Effect : public Object {
public:
	static Effect createProjectile(Vector3 position, Vector3 velocity, const SpellStats& spell);
	static Effect createMine(Vector3 position, const SpellStats& spell);
	static Effect createBurst(Vector3 position, const SpellStats& spell, float radiusOverride = -1.0f);
	static Effect createHazard(Vector3 position, const SpellStats& spell);
	static Effect createTracer(Vector3 start, Vector3 end, Color color);

	void update(float deltaTime) override;
	void draw3D() const override;
	bool isActive() const;
	EffectKind getKind() const;
	const SpellStats& getSpell() const;
	const Vector3& getVelocity() const;
	void setVelocity(Vector3 newVelocity);
	float getRadius() const;
	float getAge() const;
	float getLifetime() const;
	bool isInRadius(Vector3 point, float pointRadius = 0.0f) const;
	bool consumePulse();
	void detonate();
	void deactivate();

private:
	Effect(EffectKind kind, Vector3 position, const SpellStats& spell, float lifetime, float radius);

	EffectKind kind {EffectKind::Burst};
	SpellStats spell {};
	Vector3 velocity {};
	Vector3 tracerEnd {};
	float age {0.0f};
	float lifetime {0.0f};
	float radius {0.0f};
	float pulseTimer {0.0f};
	bool active {true};
	bool pulseReady {false};
};
