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
		float mouseWheel {};
		bool jumpPressed {false};
		bool sprintHeld {false};
		bool castPressed {false};
		bool wardHeld {false};
		bool rummageHeld {false};
		bool removeSelectionPressed {false};
		bool clearSpellPressed {false};
		bool confirmPressed {false};
		bool startPressed {false};
		bool restartPressed {false};
		bool mouseCaptured {true};
		std::array<bool, 4> basePressed {};
		std::array<bool, 6> reagentPressed {};
	};

	InputState poll();
	bool isMouseCaptured() const;
	void setMouseCaptured(bool captured);
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
