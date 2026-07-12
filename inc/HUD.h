#pragma once

class Player;

class HUD {
public:
	void draw(const Player& player, int screenWidth, int screenHeight, int wave, int enemiesAlive, bool cleared) const;
};
