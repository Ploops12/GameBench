#pragma once

#include "CurseVector.hxx"
#include "Menu.hxx"

class Player;

class CoatMenu : public Menu {
public:
	void update(Player& player);
	void draw(const Player& player) const;
	void clear();
	bool open{false};
	int cursor{0};
	CurseVector spell;
};
