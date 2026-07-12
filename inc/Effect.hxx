#pragma once

#include "CurseVector.hxx"
#include "Object.hxx"

class Effect : public Object {
public:
	Effect(Vector3 position = {}, SpellStats stats = {}, float life = 0.0f)
		: Object(position), stats(stats), life(life) {}

	SpellStats stats {};
	float life{0.0f};
	float tick{0.0f};
};
