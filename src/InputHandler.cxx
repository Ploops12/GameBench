#include "InputHandler.hxx"
#include <raylib.h>

InputHandler::InputHandler(int exitKey) {
	SetExitKey(exitKey);
	DisableCursor();
}

InputHandler::InputState InputHandler::poll() {
	InputState input;

	MovementKeys& mk = movementKeys;
	if (IsKeyDown(mk.moveForward)) input.moveInput.x += 1.0f;
	if (IsKeyDown(mk.moveBack)) input.moveInput.x -= 1.0f;
	if (IsKeyDown(mk.moveRight)) input.moveInput.y += 1.0f;
	if (IsKeyDown(mk.moveLeft)) input.moveInput.y -= 1.0f;
	if (IsKeyPressed(mk.moveUp)) input.moveInput.z = 1.0f;
	if (IsKeyDown(mk.moveDown)) input.moveInput.z -= 1.0f;

	input.sprintHeld = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
	input.rummageHeld = IsKeyDown(KEY_R);
	input.castPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	input.wardHeld = IsKeyDown(KEY_F);

	if (IsKeyPressed(mouseToggleKey)) {
		mouseCaptured = !mouseCaptured;

		if (mouseCaptured) {
			DisableCursor();
		} else {
			EnableCursor();
		}
	}

	if (mouseCaptured) input.lookInput = GetMouseDelta();

	return input;
}
