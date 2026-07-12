#pragma once

#include <string>
#include <raylib.h>

#include "Assets.hxx"

class Player;

class HUD {
	// Combat HUD, ward feedback, readied curse, notices, and always-visible hands.
public:
	void reset();
	void update(float deltaTime);
	void notify(const std::string& text, Color color = RAYWHITE);
	void triggerCast();
	void triggerHit();
	void draw(const Player& player, const GameAssets& assets, int wave, int enemiesRemaining,
		bool coatOpen, bool mouseCaptured) const;

private:
	void drawHands(const Player& player, const GameAssets& assets, bool coatOpen) const;
	std::string notice;
	Color noticeColor {RAYWHITE};
	float noticeTime {0.0f};
	float castTime {0.0f};
	float hitTime {0.0f};
};
