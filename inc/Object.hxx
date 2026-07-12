#pragma once

#include <raylib.h>

class Object {
public:
	explicit Object(Vector3 position = {0.0f, 0.0f, 0.0f});
	virtual ~Object() = default;

	virtual void update(float deltaTime);
	virtual void draw() const;

	Vector3 getPosition() const;
	void setPosition(Vector3 newPosition);
	bool isActive() const;
	void setActive(bool value);

protected:
	Vector3 position{};
	bool active{true};
};
