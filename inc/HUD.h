#pragma once

#include <array>
#include <string>
#include <raylib.h>
#include "CurseVector.hxx"

class Player;

class HUD {
public:
	HUD();
	~HUD();
	void Draw(const Player& player, const CurseVector& curse, const std::array<int, REAGENT_COUNT>& inventory,
		bool coatOpen, int kills, const std::string& message, float messageTimer) const;
	void DrawHands(bool wardActive, bool casting) const;

private:
	Texture2D hands{};
	bool handsLoaded{false};
};
