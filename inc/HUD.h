#pragma once

#include "Menu.hxx"

class Player;

class HUD : public Menu {
public:
	void draw(const Player& player, int enemiesRemaining, int collected,
		const char* message, float messageTimer, bool rummaging) const;
};
