#pragma once

#include <array>
#include <raylib.h>
#include "Reagent.hxx"

class InputHandler {
public:
	explicit InputHandler(int exitKey = KEY_NULL);

	struct InputState {
		Vector2 moveInput {};
		Vector2 lookInput {};
		bool jumpPressed{false};
		bool sprintDown{false};
		bool castPressed{false};
		bool wardDown{false};
		bool rummageHeld{false};
		bool clearPressed{false};
		bool removePressed{false};
		std::array<bool, 4> basePressed {};
		std::array<bool, static_cast<int>(ReagentType::Count)> reagentPressed {};
	};

	InputState poll();

private:
	struct MovementKeys {
		int moveForward{KEY_W};
		int moveBack{KEY_S};
		int moveRight{KEY_D};
		int moveLeft{KEY_A};
		int jump{KEY_SPACE};
		int sprint{KEY_LEFT_SHIFT};
	} movementKeys;

	int mouseToggleKey{KEY_ESCAPE};
	bool mouseCaptured{true};
};
