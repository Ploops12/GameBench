#pragma once

#include "Object.hxx"

class Actor : public Object {
public:
	Vector3 GetVelocity() const { return velocity; }
	void SetVelocity(Vector3 value) { velocity = value; }

protected:
	Vector3 velocity{};
};
