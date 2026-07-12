#pragma once

#include "Menu.hxx"

class MainMenu : public Menu {
public:
	MainMenu();
	bool updateAndCheckStart();
	void draw() const override;
};
