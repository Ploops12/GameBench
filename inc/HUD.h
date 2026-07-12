#pragma once

#include "CurseVector.hxx"
#include "Player.hxx"

class HUD {
public:
	void draw(const Player& player, const CurseVector& spell, bool coatOpen,
		const char* announcement, float announcementTime) const;

private:
	void drawMeter(int x, int y, int width, const char* label, float value, float maximum,
		Color color) const;
};
