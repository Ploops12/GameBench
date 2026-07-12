#pragma once

#include "Player.hxx"
#include <raylib.h>

class HUD {
public:
	void draw(const Player& player, int enemyCount, int screenWidth, int screenHeight) const;
};
