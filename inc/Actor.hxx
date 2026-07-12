#pragma once

#include "Object.hxx"

class Actor : public Object {
public:
	void move(Vector3 delta) { position = Vector3Add(position, delta); }
	float getRadius() const { return radius; }

protected:
	float radius{0.45f};
};
