#pragma once

#include <raylib.h>

constexpr float DEFAULT_MOVE_SPEED = 1.0f;

class InputHandler {
public:
	explicit InputHandler(int exitKey = KEY_NULL);

	struct InputState {
		Vector3 moveInput {};
		Vector2 lookInput {};
		bool sprintDown{};
		bool castPressed{};
		bool wardDown{};
		bool rummageDown{};
		bool jumpPressed{};
		bool clearPressed{};
		bool resetPressed{};
		bool mousePressed{};
		Vector2 mousePosition{};
		int selectedBase{-1};
		int selectedReagent{-1};
	};

	InputState poll();
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
	bool mouseCaptured{true};
};
