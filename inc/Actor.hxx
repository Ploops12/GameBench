#pragma once

#include "Object.hxx"
#include <raylib.h>

class Actor : public Object {
public:
	Vector3 velocity{};
	float moveSpeed{1.0f};
	bool alive{true};
};
