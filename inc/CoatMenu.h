#pragma once

#include "CurseVector.hxx"
#include "Player.hxx"
#include <raylib.h>

class CoatMenu {
public:
	bool open{false};
	int highlightedReagent{0};

	void update(Player& player);
	void draw(const Player& player, int screenWidth, int screenHeight) const;
};
