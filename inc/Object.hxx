#pragma once

#include <raylib.h>

class Object {
public:
	virtual ~Object() = default;
	virtual void Update(float dt) { (void)dt; }
	virtual void Draw() const {}

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 value) { position = value; }
	bool IsEnabled() const { return enabled; }
	void SetEnabled(bool value) { enabled = value; }
	float GetRadius() const { return radius; }

protected:
	Vector3 position{};
	float radius{0.45f};
	bool enabled{true};
};
