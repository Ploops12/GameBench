#include "InputHandler.hxx"
#include <raylib.h>

InputHandler::InputHandler(int exitKey) {
	SetExitKey(exitKey);
	if (IsWindowReady()) {
		DisableCursor();
	}
}

InputHandler::InputState InputHandler::poll() {
	InputState state;

	MovementKeys& mk = movementKeys;
	if (IsKeyDown(mk.moveForward)) state.moveInput.x += 1.0f;
	if (IsKeyDown(mk.moveBack)) state.moveInput.x -= 1.0f;
	if (IsKeyDown(mk.moveRight)) state.moveInput.y += 1.0f;
	if (IsKeyDown(mk.moveLeft)) state.moveInput.y -= 1.0f;
	if (IsKeyDown(mk.moveUp)) state.moveInput.z += 1.0f;
	if (IsKeyDown(mk.moveDown)) state.moveInput.z -= 1.0f;
	state.jumpPressed = IsKeyPressed(mk.moveUp);
	state.sprintDown = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
	state.castPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	state.wardDown = IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsKeyDown(KEY_F);
	state.rummageDown = IsKeyDown(KEY_TAB);
	state.clearPressed = IsKeyPressed(KEY_BACKSPACE);
	state.resetPressed = IsKeyPressed(KEY_R);
	state.mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	state.mousePosition = GetMousePosition();

	if (IsKeyPressed(KEY_ONE)) state.selectedBase = 0;
	if (IsKeyPressed(KEY_TWO)) state.selectedBase = 1;
	if (IsKeyPressed(KEY_THREE)) state.selectedBase = 2;
	if (IsKeyPressed(KEY_FOUR)) state.selectedBase = 3;

	if (IsKeyPressed(KEY_Z)) state.selectedReagent = 0;
	if (IsKeyPressed(KEY_X)) state.selectedReagent = 1;
	if (IsKeyPressed(KEY_C)) state.selectedReagent = 2;
	if (IsKeyPressed(KEY_V)) state.selectedReagent = 3;
	if (IsKeyPressed(KEY_B)) state.selectedReagent = 4;
	if (IsKeyPressed(KEY_N)) state.selectedReagent = 5;

	if (IsKeyPressed(mouseToggleKey)) {
		mouseCaptured = !mouseCaptured;

		if (mouseCaptured) {
			if (IsWindowReady()) DisableCursor();
		} else {
			if (IsWindowReady()) EnableCursor();
		}
	}
	if (mouseCaptured && IsWindowReady()) {
		if (state.rummageDown) {
			EnableCursor();
		} else {
			DisableCursor();
		}
	}

	if (mouseCaptured && !state.rummageDown) {
		state.lookInput = GetMouseDelta();
	}

	return state;
}

bool InputHandler::isMouseCaptured() const {
	return mouseCaptured;
}
