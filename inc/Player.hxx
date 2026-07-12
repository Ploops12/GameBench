#pragma once

#include <raylib.h>
#include <vector>
#include "Actor.hxx"
#include "InputHandler.hxx"

class Player : public Actor {
	Camera3D camera{};
	Vector3 velocity{};
	Vector3 previousPosition{};
	float yaw{-90.0f};
	float pitch{};
	float health{100.0f};
	float ward{100.0f};
	bool wardActive{};
public:
	Player();
	void update(const InputHandler::InputState& input, float dt, bool rummaging);
	void constrain(float halfExtent);
	void resolveCollisions(const std::vector<BoundingBox>& obstacles);
	void damage(float amount);
	void restoreWard(float amount);
	void toggleWard();
	Camera3D& getCamera() { return camera; }
	const Camera3D& getCamera() const { return camera; }
	float getHealth() const { return health; }
	float getWard() const { return ward; }
	bool isWardActive() const { return wardActive; }
// This class owns the first-person view, movement, health, ward, and camera.
};
