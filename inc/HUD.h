#pragma once

#include <string>

#include "CoatMenu.h"
#include "Player.hxx"
#include "Reagent.hxx"
#include "Sprite.hxx"

class HUD {
public:
	void draw(const Player& player, const CoatMenu& coatMenu, const ReagentInventory& inventory,
		const Sprite* hands, int livingEnemies, const std::string& notification, float notificationTime) const;
};
