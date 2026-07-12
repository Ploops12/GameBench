#pragma once

#include "CurseVector.hxx"
#include "Object.hxx"

enum class EffectKind {
	Burst,
	LingeringRot,
	SnareSigil,
	FearPulse,
	WardPulse,
	HitSpark
};

class Effect : public Object {
	// Short-lived burst/sigil or a lingering area that can pulse its spell stats.
public:
	Effect(EffectKind kind, Vector3 position, float radius, float duration, Color color,
		const SpellStats& spellStats = {});

	void update(float deltaTime) override;
	void draw3D(const Camera3D& camera) const override;
	bool contains(Vector3 point) const;
	bool takePulse();
	EffectKind getKind() const;
	const SpellStats& getSpellStats() const;
	float getRemainingTime() const;

private:
	EffectKind kind;
	float radius;
	float duration;
	float remainingTime;
	float age {0.0f};
	float pulseAccumulator {0.0f};
	bool pulseReady {false};
	Color color;
	SpellStats spellStats;
};
