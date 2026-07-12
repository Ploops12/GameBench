#pragma once

#include <raylib.h>

constexpr float DEFAULT_MOVE_SPEED = 1.0f;

class InputHandler {
public:
	explicit InputHandler(int exitKey = KEY_NULL);

	struct InputState {
		Vector3 moveInput {};
		Vector2 lookInput {};
		bool jumpPressed {false};
		bool sprintHeld {false};
		bool wardHeld {false};
		bool castPressed {false};
		bool interactPressed {false};
		bool rummageHeld {false};
		bool rummageTogglePressed {false};
		bool confirmPressed {false};
		bool removeReagentPressed {false};
		bool clearReagentsPressed {false};
		int quickKey {-1};
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
		int moveUp{KEY_SPACE};
		int moveDown{KEY_LEFT_CONTROL};
	} movementKeys;

	int mouseToggleKey{KEY_ESCAPE};
	bool mouseCaptured{false};
};
