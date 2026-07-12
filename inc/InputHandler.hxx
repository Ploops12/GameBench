#pragma once

#include <array>
#include <raylib.h>

constexpr float DEFAULT_MOVE_SPEED = 1.0f;

class InputHandler {
public:
	explicit InputHandler(int exitKey = KEY_NULL);

	struct InputState {
		Vector3 moveInput {};
		Vector2 lookInput {};
		Vector2 mousePosition {};
		bool jumpPressed {false};
		bool sprintHeld {false};
		bool castPressed {false};
		bool wardHeld {false};
		bool rummageHeld {false};
		bool rummageTogglePressed {false};
		bool menuCastPressed {false};
		bool removeReagentPressed {false};
		bool clearRecipePressed {false};
		bool baseBottlePressed {false};
		bool baseApplePressed {false};
		bool baseSpiderPressed {false};
		bool basePoppetPressed {false};
		std::array<bool, 6> reagentPressed {};
		bool startPressed {false};
		bool mouseTogglePressed {false};
	};

	InputState poll();
	void setMouseCaptured(bool captured);
	bool isMouseCaptured() const;
private:
	struct MovementKeys {
		int moveForward{KEY_W};
		int moveBack{KEY_S};
		int moveRight{KEY_D};
		int moveLeft{KEY_A};
	} movementKeys;

	int mouseToggleKey{KEY_ESCAPE};
	bool mouseCaptured{true};
};
