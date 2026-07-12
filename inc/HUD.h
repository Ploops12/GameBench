#pragma once

#include "Player.hxx"

class HUD {
public:
	void draw(const Player &player, bool coatOpen, int enemiesRemaining) const;
};
