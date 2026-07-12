#include "InputHandler.hxx"

InputHandler::InputHandler(int exitKey) {
	SetExitKey(exitKey);
	DisableCursor();
}

InputHandler::~InputHandler() {
	if (mouseCaptured) {
		EnableCursor();
	}
}

InputHandler::InputState InputHandler::poll() {
	InputState is;
	bool capturedThisFrame = false;

	MovementKeys& mk = movementKeys;
	if (IsKeyDown(mk.moveForward)) is.moveInput.x += 1.0f;
	if (IsKeyDown(mk.moveBack)) is.moveInput.x -= 1.0f;
	if (IsKeyDown(mk.moveRight)) is.moveInput.y += 1.0f;
	if (IsKeyDown(mk.moveLeft)) is.moveInput.y -= 1.0f;
	if (IsKeyDown(mk.moveUp)) is.moveInput.z += 1.0f;
	if (IsKeyDown(mk.moveDown)) is.moveInput.z -= 1.0f;
	is.jumpPressed = IsKeyPressed(mk.moveUp);
	is.sprintHeld = IsKeyDown(KEY_LEFT_SHIFT);
	is.coatHeld = IsKeyDown(KEY_R);
	is.coatTogglePressed = IsKeyPressed(KEY_TAB);
	is.restartPressed = IsKeyPressed(KEY_ENTER);
	is.mousePosition = GetMousePosition();
	is.menuClickPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

	if (IsKeyPressed(KEY_ONE)) {
		is.coatVectorSelection = 0;
		is.mainVectorSelection = 0;
	}
	if (IsKeyPressed(KEY_TWO)) {
		is.coatVectorSelection = 1;
		is.mainVectorSelection = 1;
	}
	if (IsKeyPressed(KEY_THREE)) {
		is.coatVectorSelection = 2;
		is.mainVectorSelection = 2;
	}
	if (IsKeyPressed(KEY_FOUR)) is.coatVectorSelection = 3;
	if (IsKeyPressed(KEY_ZERO)) is.coatVectorSelection = 3;

	is.mainVectorPreviousPressed = IsKeyPressed(KEY_Q);
	is.mainVectorNextPressed = IsKeyPressed(KEY_E);

	is.coatRemoveHeld = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
	is.coatUndoPressed = IsKeyPressed(KEY_BACKSPACE);
	is.coatClearPressed = IsKeyPressed(KEY_DELETE);
	is.coatCastPressed = IsKeyPressed(KEY_ENTER);

	if (IsKeyPressed(mouseToggleKey)) {
		const bool capture = !mouseCaptured;
		setMouseCaptured(capture);
		capturedThisFrame = capture;
		is.pausePressed = true;
	}

	if (mouseCaptured) {
		is.lookInput = capturedThisFrame ? Vector2 {} : GetMouseDelta();
		is.castPressed = !capturedThisFrame
			&& IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
		is.wardHeld = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
	}

	return is;
}

bool InputHandler::isMouseCaptured() const {
	return mouseCaptured;
}

void InputHandler::setMouseCaptured(bool captured) {
	mouseCaptured = captured;
	if (mouseCaptured) {
		DisableCursor();
	} else {
		EnableCursor();
	}
}
