#pragma once

#include "Reagent.hxx"

#include <array>
#include <raylib.h>

constexpr float DEFAULT_MOVE_SPEED = 1.0f;

class InputHandler {
public:
	explicit InputHandler(int exitKey = KEY_NULL);

	struct InputState {
		Vector3 moveInput {};
		Vector2 lookInput {};
		bool jumpPressed {false};
		bool sprintHeld {false};
		bool rummageHeld {false};
		bool castPressed {false};
		bool wardHeld {false};
		bool undoPressed {false};
		bool clearPressed {false};
		bool confirmPressed {false};
		bool mouseCaptured {true};
		int baseSelection {-1};
		std::array<bool, REAGENT_TYPE_COUNT> reagentPressed {};
	};

	[[nodiscard]] InputState poll();
	[[nodiscard]] bool isMouseCaptured() const;
	void captureMouse(bool captured);

private:
	struct MovementKeys {
		int moveForward {KEY_W};
		int moveBack {KEY_S};
		int moveRight {KEY_D};
		int moveLeft {KEY_A};
		int moveUp {KEY_SPACE};
		int moveDown {KEY_LEFT_CONTROL};
	} movementKeys_;

	int mouseToggleKey_ {KEY_ESCAPE};
	bool mouseCaptured_ {true};
};
