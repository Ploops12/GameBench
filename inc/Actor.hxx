#pragma once

#include "Object.hxx"

class Actor : public Object {
public:
	using Object::Object;
	Vector3 velocity {};
	float health{100.0f};
	float maxHealth{100.0f};
};
