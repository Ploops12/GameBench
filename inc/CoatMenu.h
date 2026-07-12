#pragma once

#include "Assets.hxx"
#include "CurseVector.hxx"
#include "InputHandler.hxx"
#include "Menu.hxx"

class Player;

class CoatMenu : public Menu {
	// Hold-to-rummage editor. Its draft becomes the player's readied hotbar curse on release.
public:
	CoatMenu();
	using Menu::draw;
	using Menu::update;
	void reset();
	void update(const InputHandler::InputState& input, Player& player);
	void draw(const Player& player, const GameAssets& assets) const;
	const CurseVector& getDraft() const;
	bool wasCommitted();

private:
	void handleSelection(const InputHandler::InputState& input, Player& player);
	CurseVector draft;
	bool committed {false};
};
