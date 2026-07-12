#pragma once

#include "InputHandler.hxx"
#include "Menu.hxx"

class MainMenu : public Menu {
public:
	MainMenu();
	[[nodiscard]] bool update(const InputHandler::InputState& input);
	void draw() const override;

private:
	float age_ {0.0f};
};
