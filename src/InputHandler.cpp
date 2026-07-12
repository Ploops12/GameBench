#include "InputHandler.hxx"

InputHandler::InputHandler(int exitKey) {
	SetExitKey(exitKey);
	setMouseCaptured(true);
}

InputHandler::InputState InputHandler::poll() {
	InputState state {};

	MovementKeys& keys = movementKeys;
	if (IsKeyDown(keys.moveForward)) state.moveInput.x += 1.0f;
	if (IsKeyDown(keys.moveBack)) state.moveInput.x -= 1.0f;
	if (IsKeyDown(keys.moveRight)) state.moveInput.y += 1.0f;
	if (IsKeyDown(keys.moveLeft)) state.moveInput.y -= 1.0f;

	if (IsKeyPressed(mouseToggleKey)) {
		setMouseCaptured(!mouseCaptured);
	}

	state.lookInput = mouseCaptured ? GetMouseDelta() : Vector2 {};
	state.jumpPressed = IsKeyPressed(keys.moveUp);
	state.sprintHeld = IsKeyDown(KEY_LEFT_SHIFT);
	state.wardHeld = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
	state.castPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	state.interactPressed = IsKeyPressed(KEY_E);
	state.rummageHeld = IsKeyDown(KEY_R);
	state.rummageTogglePressed = IsKeyPressed(KEY_TAB);
	state.confirmPressed = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
		IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_SPACE);
	state.removeReagentPressed = IsKeyPressed(KEY_BACKSPACE);
	state.clearReagentsPressed = IsKeyPressed(KEY_C);

	const int quickKeys[] {
		KEY_ZERO, KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR,
		KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE
	};
	for (int index = 0; index < 10; ++index) {
		if (IsKeyPressed(quickKeys[index])) {
			state.quickKey = index;
			break;
		}
	}

	return state;
}

void InputHandler::setMouseCaptured(bool captured) {
	if (mouseCaptured == captured) {
		return;
	}

	mouseCaptured = captured;
	if (mouseCaptured) {
		DisableCursor();
	} else {
		EnableCursor();
	}
}

bool InputHandler::isMouseCaptured() const {
	return mouseCaptured;
}
