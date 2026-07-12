#pragma once

// Real-time coat rummage UI for selecting one base vector plus up to three stackable reagents while combat continues.

#include "InputHandler.hxx"
#include "Player.hxx"

class CoatMenu {
public:
	void update(const InputHandler::InputState& input, Player& player);
	void draw(const Player& player) const;
	bool isOpen() const;

private:
	bool open{};
	float pulse{};
};
