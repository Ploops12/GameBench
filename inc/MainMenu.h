#pragma once

#include "InputHandler.hxx"
#include "Menu.hxx"

class MainMenu : public Menu {
public:
	MainMenu();
	bool update(const InputHandler::InputState& input);
	void draw(int screenWidth, int screenHeight) const override;
};
