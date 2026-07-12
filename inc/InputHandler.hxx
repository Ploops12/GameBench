#pragma once

#include <raylib.h>

constexpr float DEFAULT_MOVE_SPEED = 1.0f;

class InputHandler {
public:
	explicit InputHandler(int exitKey = KEY_NULL);
	~InputHandler();

	struct InputState {
		Vector3 moveInput {};
		Vector2 lookInput {};
		bool jumpPressed {false};
		bool sprintHeld {false};
		bool coatHeld {false};
		bool castPressed {false};
		bool wardHeld {false};
		bool restartPressed {false};
		bool pausePressed {false};
		int coatVectorSelection {-1};
		int coatReagentSelection {-1};
		bool coatRemoveHeld {false};
		bool coatUndoPressed {false};
		bool coatClearPressed {false};
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
