#include "InputHandler.hxx"

InputHandler::InputHandler(int exitKey) {
	SetExitKey(exitKey);
}

InputHandler::InputState InputHandler::poll() {
	InputState input;

	MovementKeys& mk = movementKeys;
	if (IsKeyDown(mk.moveForward)) input.moveInput.x += 1.0f;
	if (IsKeyDown(mk.moveBack)) input.moveInput.x -= 1.0f;
	if (IsKeyDown(mk.moveRight)) input.moveInput.y += 1.0f;
	if (IsKeyDown(mk.moveLeft)) input.moveInput.y -= 1.0f;

	input.mousePosition = GetMousePosition();
	input.jumpPressed = IsKeyPressed(KEY_SPACE);
	input.sprintHeld = IsKeyDown(KEY_LEFT_SHIFT);
	input.castPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	input.wardHeld = IsKeyDown(KEY_F) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
	input.rummageHeld = IsKeyDown(KEY_R);
	input.rummageTogglePressed = IsKeyPressed(KEY_TAB);
	input.menuCastPressed = IsKeyPressed(KEY_ENTER);
	input.removeReagentPressed = IsKeyPressed(KEY_BACKSPACE);
	input.clearRecipePressed = IsKeyPressed(KEY_DELETE);
	input.baseBottlePressed = IsKeyPressed(KEY_ONE);
	input.baseApplePressed = IsKeyPressed(KEY_TWO);
	input.baseSpiderPressed = IsKeyPressed(KEY_THREE);
	input.basePoppetPressed = IsKeyPressed(KEY_FOUR);
	input.reagentPressed = {
		IsKeyPressed(KEY_Z), IsKeyPressed(KEY_X), IsKeyPressed(KEY_C),
		IsKeyPressed(KEY_V), IsKeyPressed(KEY_B), IsKeyPressed(KEY_N)
	};
	input.startPressed = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || input.castPressed;
	input.mouseTogglePressed = IsKeyPressed(mouseToggleKey);

	if (input.mouseTogglePressed) {
		mouseCaptured = !mouseCaptured;

		if (mouseCaptured) {
			DisableCursor();
		} else {
			EnableCursor();
		}
	}

	if (mouseCaptured) {
		input.lookInput = GetMouseDelta();
	}

	return input;
}

void InputHandler::setMouseCaptured(bool captured) {
	mouseCaptured = captured;
	if (mouseCaptured) {
		DisableCursor();
	} else {
		EnableCursor();
	}

	GetMouseDelta();
}

bool InputHandler::isMouseCaptured() const {
	return mouseCaptured;
}
