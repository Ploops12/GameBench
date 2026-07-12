#pragma once

#include "Reagent.hxx"
#include <raylib.h>

struct Pickup {
	ReagentType type{ReagentType::GraveSalt};
	Vector3 position{};
	float bobSeed{0.0f};
	bool active{true};
};
