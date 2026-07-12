#pragma once

#include <raylib.h>
#include "Player.hxx"

class CoatMenu {
public:
	bool update(Player &player);
	void draw(const Player &player) const;
	bool isOpen() const { return open; }

private:
	bool open{false};
	int cursor{0};
	int baseCursor{0};
};
