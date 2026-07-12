#pragma once

#include "Menu.hxx"

class MainMenu : public Menu {
public:
	void update();
	void draw() const;
	bool takePlayRequest();

private:
	bool playRequested {};
	float pulse {};
};
