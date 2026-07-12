#pragma once

#include <raylib.h>

class Actor {
public:
	virtual ~Actor() = default;
	Vector3 position{};
	float radius{0.45f};
};
