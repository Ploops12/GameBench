#include "InputHandler.hxx"

InputHandler::InputHandler(int exitKey) {
	SetExitKey(exitKey);
	setMouseCaptured(true);
}

void InputHandler::setMouseCaptured(bool captured) {
	mouseCaptured = captured;
	if (mouseCaptured) DisableCursor();
	else EnableCursor();
}

InputHandler::InputState InputHandler::poll() {
	InputState state;

	if (IsKeyDown(movementKeys.moveForward)) state.moveInput.x += 1.0f;
	if (IsKeyDown(movementKeys.moveBack)) state.moveInput.x -= 1.0f;
	if (IsKeyDown(movementKeys.moveRight)) state.moveInput.y += 1.0f;
	if (IsKeyDown(movementKeys.moveLeft)) state.moveInput.y -= 1.0f;
	state.jumpPressed = IsKeyPressed(KEY_SPACE);
	state.sprintDown = IsKeyDown(KEY_LEFT_SHIFT);
	state.rummageDown = IsKeyDown(KEY_Q);
	state.wardDown = IsKeyDown(KEY_F);
	state.castPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

	if (IsKeyPressed(mouseToggleKey)) {
		setMouseCaptured(!mouseCaptured);
	}
	if (mouseCaptured) state.lookInput = GetMouseDelta();

	return state;
}
