#pragma once

#include "Menu.hxx"

class Player;

class CoatMenu : public Menu {
public:
	void update(Player& player);
	void draw(const Player& player) const;
	bool isOpen() const { return isActive(); }
	void setOpen(bool value) { setActive(value); }

private:
	float pulse {};
};
