#pragma once

// Main player HUD: health, ward, active spell preview, reagent quantities, controls, and combat feedback.

#include "CoatMenu.h"
#include "Enemy.hxx"
#include "Player.hxx"

#include <vector>

class HUD {
public:
	void draw(const Player& player, const CoatMenu& coatMenu, const std::vector<Enemy>& enemies, int livingEnemies) const;
};
