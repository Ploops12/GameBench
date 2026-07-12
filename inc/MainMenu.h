#pragma once

#include "InputHandler.hxx"
#include "Menu.hxx"

class MainMenu : public Menu {
public:
	bool update(const InputHandler::InputState& input);
	void draw(int screenWidth, int screenHeight) const;
	bool isOpen() const override;

private:
	bool open {true};
};
