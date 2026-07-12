#include "InputHandler.hxx"
#include <raylib.h>

InputHandler::InputHandler(int exitKey) {
	SetExitKey(exitKey);
}

void InputHandler::SetMouseCaptured(bool captured) {
	mouseCaptured = captured;
	if (mouseCaptured) DisableCursor();
	else EnableCursor();
}

InputHandler::InputState InputHandler::poll() {
	InputState is;

	MovementKeys& mk = movementKeys;
	if (IsKeyDown(mk.moveForward)) is.moveInput.x += 1.0f;
	if (IsKeyDown(mk.moveBack)) is.moveInput.x -= 1.0f;
	if (IsKeyDown(mk.moveRight)) is.moveInput.y += 1.0f;
	if (IsKeyDown(mk.moveLeft)) is.moveInput.y -= 1.0f;
	if (IsKeyDown(mk.moveUp)) is.moveInput.z += 1.0f;
	if (IsKeyDown(mk.moveDown)) is.moveInput.z -= 1.0f;
	is.jumpPressed = IsKeyPressed(mk.moveUp);
	is.sprintHeld = IsKeyDown(KEY_LEFT_SHIFT);
	is.castPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ENTER);
	is.wardPressed = IsKeyPressed(KEY_F);
	is.coatPressed = IsKeyPressed(KEY_R);
	is.coatHeld = IsKeyDown(KEY_TAB);
	is.escapePressed = IsKeyPressed(KEY_ESCAPE);

	if (IsKeyPressed(mouseToggleKey)) {
		SetMouseCaptured(!mouseCaptured);
	}
	if (mouseCaptured) is.lookInput = GetMouseDelta();

	return is;
}
