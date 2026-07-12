#pragma once

#include <string>

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
	bool takeCastRequest();
	void completeCast(Player& player);

private:
	void handleSelection(const InputHandler::InputState& input, Player& player);
	void setBase(CurseVectorType type);
	bool tryAddReagent(ReagentType type, const ReagentInventory& inventory);
	int selectedCount(ReagentType type) const;
	CurseVector draft;
	bool toggledOpen {false};
	bool committed {false};
	bool castRequested {false};
	std::string statusLine {"Choose a base, then stitch up to three stacks."};
};
