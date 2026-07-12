#pragma once

#include <raylib.h>
#include "Actor.hxx"
#include "InputHandler.hxx"
#include "Reagent.hxx"

class Player : public Actor {
public:
	using Actor::update;
	Player();
	void update(float dt, const InputHandler::InputState& input, bool rummaging);
	void damage(float amount);
	void reset();
	Vector3 forward() const;
	Vector3 eyePosition() const;

	Camera3D camera {};
	ReagentInventory inventory {{3, 3, 2, 2, 1, 2}};
	float yaw{0.0f};
	float pitch{0.0f};
	float ward{100.0f};
	float maxWard{100.0f};
	float wardRegenDelay{0.0f};
	bool wardActive{false};
	bool grounded{true};
};
