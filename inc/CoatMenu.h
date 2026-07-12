#pragma once

#include "Menu.hxx"
#include "InputHandler.hxx"

class Player;

class CoatMenu : public Menu {
public:
	void update(Player& player, const InputHandler::InputState& input);
	void draw(const Player& player, int screenWidth, int screenHeight) const;
};
