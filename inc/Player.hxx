#pragma once

#include <raylib.h>
#include "Actor.h"

class Player : public Actor {
	Camera3D camera;
	Vector3 position;
// This class needs to support the main view camera, movement, player actions / control handling (using InputHandler for getting used inputs), stats (like health), and inventory
};
