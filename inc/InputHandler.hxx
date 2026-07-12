#pragma once

constexpr float DEFAULT_MOVE_SPEED = 1.0f;

class InputHandler {
public:
	explicit InputHandler(int exitKey = KEY_NULL);

	struct InputState {
		Vector3 moveInput {};
		Vector2 lookInput {};
	};

	InputState poll();
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
	bool mouseCaptured{true};
};
