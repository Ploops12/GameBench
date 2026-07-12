#pragma once

#include "CurseVector.hxx"

class Effect {
public:
	Effect(Vector3 position, float radius, float duration, const SpellStats& spell);

	bool update(float deltaTime);
	bool isAlive() const;
	void draw3D() const;
	Vector3 getPosition() const;
	float getRadius() const;
	const SpellStats& getSpell() const;

private:
	Vector3 position {};
	float radius {};
	float timeLeft {};
	float totalTime {};
	SpellStats spell {};
};
