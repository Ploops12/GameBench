#pragma once

#include <raylib.h>

#include <string>

class Player;
class CurseVector;

class HUD {
public:
	void draw(const Player& player, const CurseVector& spell, bool rummaging, bool castReady, const std::string& status) const;
	void drawCrosshair(bool wardActive) const;
};
