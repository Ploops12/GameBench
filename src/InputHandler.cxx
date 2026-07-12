#include "InputHandler.hxx"

#include <raylib.h>

InputHandler::InputHandler(int exitKey) {
	SetExitKey(exitKey);
}

InputHandler::InputState InputHandler::poll() {
	InputState state{};
	MovementKeys& keys = movementKeys;

	if (IsKeyDown(keys.moveForward)) state.moveInput.x += 1.0f;
	if (IsKeyDown(keys.moveBack)) state.moveInput.x -= 1.0f;
	if (IsKeyDown(keys.moveRight)) state.moveInput.y += 1.0f;
	if (IsKeyDown(keys.moveLeft)) state.moveInput.y -= 1.0f;
	if (IsKeyDown(keys.moveUp)) state.moveInput.z += 1.0f;
	if (IsKeyDown(keys.moveDown)) state.moveInput.z -= 1.0f;

	if (IsKeyPressed(mouseToggleKey)) setMouseCaptured(!mouseCaptured);
	if (mouseCaptured) state.lookInput = GetMouseDelta();
	state.mousePosition = GetMousePosition();

	const int numberKeys[6] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX};
	for (int i = 0; i < 6; ++i) state.numberPressed[i] = IsKeyPressed(numberKeys[i]);

	state.jumpPressed = IsKeyPressed(keys.moveUp);
	state.sprintHeld = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
	state.castPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	state.castHeld = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
	state.mouseLeftPressed = state.castPressed;
	state.wardPressed = IsKeyPressed(KEY_F);
	state.wardHeld = IsKeyDown(KEY_F);
	state.rummageHeld = IsKeyDown(KEY_R);
	state.rummageTogglePressed = IsKeyPressed(KEY_TAB);
	state.previousBasePressed = IsKeyPressed(KEY_Q);
	state.nextBasePressed = IsKeyPressed(KEY_E);
	state.clearSelectionPressed = IsKeyPressed(KEY_BACKSPACE);

	return state;
}

bool InputHandler::mouseIsCaptured() const {
	return mouseCaptured;
}

void InputHandler::setMouseCaptured(bool captured) {
	mouseCaptured = captured;
	if (!IsWindowReady()) return;
	if (mouseCaptured) {
		DisableCursor();
		GetMouseDelta();
	} else {
		EnableCursor();
	}
}
