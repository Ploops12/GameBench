#include "InputHandler.hxx"
#include <raylib.h>

InputHandler::InputHandler(int exitKey) {
	SetExitKey(exitKey);
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

	if (IsKeyPressed(mouseToggleKey)) {
		mouseCaptured_ = !mouseCaptured_;

		if (mouseCaptured_) {
			DisableCursor();
			is.lookInput = GetMouseDelta();
		} else {
			EnableCursor();
		}
	}
	if (mouseCaptured_) is.lookInput = GetMouseDelta();

	return is;
}
