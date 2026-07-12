#pragma once

#include "Menu.hxx"

class Player;
class CoatMenu;

class HUD : public Menu {
public:
	void draw(const Player& player, const CoatMenu& coat, int enemies, float messageTime, const char* message) const;
};
