#pragma once

class Player;
class CoatMenu;

class HUD {
public:
	void draw(const Player& player, const CoatMenu& coat, int enemiesRemaining) const;
	void drawHands(const Player& player, bool casting) const;
};
