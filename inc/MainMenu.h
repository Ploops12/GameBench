#pragma once

#include "Menu.hxx"

class MainMenu : public Menu {
public:
	void setCanStart(bool value);
	bool canStart() const;

private:
	bool startEnabled{true};
};
