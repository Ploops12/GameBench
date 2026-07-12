#include "InputHandler.hxx"

InputHandler::InputHandler(int exitKey) {
	if (IsWindowReady()) {
		SetExitKey(exitKey);
	}
	captureMouse(true);
}

InputHandler::InputState InputHandler::poll() {
	InputState input;

	if (IsKeyPressed(mouseToggleKey_)) {
		captureMouse(!mouseCaptured_);
	}

	if (IsKeyDown(movementKeys_.moveForward)) input.moveInput.x += 1.0f;
	if (IsKeyDown(movementKeys_.moveBack)) input.moveInput.x -= 1.0f;
	if (IsKeyDown(movementKeys_.moveRight)) input.moveInput.y += 1.0f;
	if (IsKeyDown(movementKeys_.moveLeft)) input.moveInput.y -= 1.0f;
	if (IsKeyDown(movementKeys_.moveUp)) input.moveInput.z += 1.0f;
	if (IsKeyDown(movementKeys_.moveDown)) input.moveInput.z -= 1.0f;

	input.mouseCaptured = mouseCaptured_;
	if (mouseCaptured_) {
		input.lookInput = GetMouseDelta();
		input.castPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
		input.wardHeld = IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsKeyDown(KEY_Q);
	}
	input.jumpPressed = IsKeyPressed(movementKeys_.moveUp);
	input.sprintHeld = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
	input.rummageHeld = IsKeyDown(KEY_R);
	input.undoPressed = IsKeyPressed(KEY_BACKSPACE);
	input.clearPressed = IsKeyPressed(KEY_DELETE);
	input.confirmPressed = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);

	if (IsKeyPressed(KEY_ONE)) input.baseSelection = 0;
	if (IsKeyPressed(KEY_TWO)) input.baseSelection = 1;
	if (IsKeyPressed(KEY_THREE)) input.baseSelection = 2;
	if (IsKeyPressed(KEY_FOUR)) input.baseSelection = 3;

	const auto& definitions = GetReagentDefinitions();
	for (std::size_t i = 0; i < definitions.size(); ++i) {
		input.reagentPressed[i] = IsKeyPressed(definitions[i].selectionKey);
	}

	return input;
}

bool InputHandler::isMouseCaptured() const {
	return mouseCaptured_;
}

void InputHandler::captureMouse(bool captured) {
	mouseCaptured_ = captured;
	if (!IsWindowReady()) {
		return;
	}
	if (mouseCaptured_) {
		DisableCursor();
		SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
	} else {
		EnableCursor();
	}
}
