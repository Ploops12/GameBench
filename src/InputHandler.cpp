#include "InputHandler.hxx"
#include "Reagent.hxx"

InputHandler::InputHandler(int exitKey) {
	SetExitKey(exitKey);
	if (IsWindowReady()) {
		DisableCursor();
	}
}

InputHandler::InputState InputHandler::poll() {
	InputState state;

	MovementKeys& keys = movementKeys;
	if (IsKeyDown(keys.moveForward)) state.moveInput.x += 1.0f;
	if (IsKeyDown(keys.moveBack)) state.moveInput.x -= 1.0f;
	if (IsKeyDown(keys.moveRight)) state.moveInput.y += 1.0f;
	if (IsKeyDown(keys.moveLeft)) state.moveInput.y -= 1.0f;
	if (IsKeyDown(keys.moveUp)) state.moveInput.z += 1.0f;
	if (IsKeyDown(keys.moveDown)) state.moveInput.z -= 1.0f;

	if (IsKeyPressed(mouseToggleKey)) {
		setMouseCaptured(!mouseCaptured);
	}

	state.mouseCaptured = mouseCaptured;
	if (mouseCaptured) {
		state.lookInput = GetMouseDelta();
	}

	state.mouseWheel = GetMouseWheelMove();
	state.jumpPressed = IsKeyPressed(KEY_SPACE);
	state.sprintHeld = IsKeyDown(KEY_LEFT_SHIFT);
	state.castPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	state.wardHeld = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
	state.rummageHeld = IsKeyDown(KEY_R);
	state.removeSelectionPressed = IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_X);
	state.clearSpellPressed = IsKeyPressed(KEY_C);
	state.confirmPressed = IsKeyPressed(KEY_ENTER);
	state.startPressed = IsKeyPressed(KEY_ENTER);
	state.restartPressed = IsKeyPressed(KEY_ENTER);

	for (int i = 0; i < 4; ++i) {
		state.basePressed[i] = IsKeyPressed(KEY_ONE + i);
	}

	for (int i = 0; i < ReagentTypeCount; ++i) {
		state.reagentPressed[i] = IsKeyPressed(GetReagentDescriptor(static_cast<std::size_t>(i)).key);
	}

	return state;
}

bool InputHandler::isMouseCaptured() const {
	return mouseCaptured;
}

void InputHandler::setMouseCaptured(bool captured) {
	mouseCaptured = captured;
	if (!IsWindowReady()) {
		return;
	}
	if (mouseCaptured) {
		DisableCursor();
	} else {
		EnableCursor();
	}
}
