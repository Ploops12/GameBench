#include "InputHandler.hxx"

InputHandler::InputHandler(int exitKey) {
	SetExitKey(exitKey);
	if (IsWindowReady()) {
		DisableCursor();
	}
}

InputHandler::InputState InputHandler::poll() {
	InputState state;

	if (IsKeyPressed(mouseToggleKey)) {
		mouseCaptured = !mouseCaptured;
		if (mouseCaptured) {
			if (IsWindowReady()) {
				DisableCursor();
			}
		} else {
			if (IsWindowReady()) {
				EnableCursor();
			}
		}
	}

	if (mouseCaptured) {
		state.lookInput = GetMouseDelta();
	}

	if (IsKeyDown(movementKeys.moveForward)) state.moveInput.y += 1.0f;
	if (IsKeyDown(movementKeys.moveBack)) state.moveInput.y -= 1.0f;
	if (IsKeyDown(movementKeys.moveRight)) state.moveInput.x += 1.0f;
	if (IsKeyDown(movementKeys.moveLeft)) state.moveInput.x -= 1.0f;

	state.jumpPressed = IsKeyPressed(movementKeys.jump);
	state.sprintDown = IsKeyDown(movementKeys.sprint);
	state.castPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	state.wardDown = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
	state.rummageHeld = IsKeyDown(KEY_TAB);
	state.clearPressed = IsKeyPressed(KEY_C);
	state.removePressed = IsKeyPressed(KEY_BACKSPACE);

	for (int i = 0; i < 4; ++i) {
		state.basePressed[i] = IsKeyPressed(KEY_ONE + i);
	}

	const auto& infos = getReagentInfos();
	for (int i = 0; i < static_cast<int>(infos.size()); ++i) {
		state.reagentPressed[i] = IsKeyPressed(infos[i].keybind);
	}

	return state;
}
