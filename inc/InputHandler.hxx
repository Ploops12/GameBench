#pragma once

#include <raylib.h>

class InputHandler {
public:
	explicit InputHandler(int exitKey = KEY_NULL);

	struct InputState {
		Vector3 moveInput {};
		Vector2 lookInput {};
		bool jumpPressed {};
		bool sprintDown {};
		bool rummageDown {};
		bool wardDown {};
		bool castPressed {};
	};

	InputState poll();
	bool isMouseCaptured() const { return mouseCaptured; }
	void setMouseCaptured(bool captured);
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
