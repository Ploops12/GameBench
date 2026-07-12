#pragma once

#include "CurseVector.hxx"
#include "Player.hxx"

#include <string>

class HUD {
public:
	void update(float dt);
	void setNotice(std::string notice, float duration = 2.0f);
	void registerKill();

	void draw(const Player& player, const CurseVector& spell, bool rummaging,
		int wave, int enemiesRemaining, int pickupsRemaining) const;
	void drawHands(const Player& player, bool rummaging) const;
	void drawTerminalOverlay(bool victory, int kills) const;

private:
	std::string notice_;
	float noticeTimer_ {0.0f};
	float killFlash_ {0.0f};
};
