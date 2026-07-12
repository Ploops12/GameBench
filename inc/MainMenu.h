#pragma once

#include "Menu.hxx"
#include "InputHandler.hxx"

class MainMenu : public Menu {
public:
	bool update(const InputHandler::InputState& input);
	void draw(int screenWidth, int screenHeight) const;
};
