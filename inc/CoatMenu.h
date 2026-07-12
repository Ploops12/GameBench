#pragma once

#include <string>

#include "CurseVector.hxx"
#include "InputHandler.hxx"
#include "Menu.hxx"

class CoatMenu : public Menu {
public:
	void update(const InputHandler::InputState& input, const ReagentInventory& inventory,
		int screenWidth, int screenHeight);
	void draw(const ReagentInventory& inventory, int screenWidth, int screenHeight) const;

	bool isOpen() const override;
	bool takeCastRequest();
	bool consumeForCast(ReagentInventory& inventory);
	const CurseVector& getCurseVector() const;
	CurseVector& getCurseVector();

private:
	void setBase(CurseVectorType base);
	bool tryAddReagent(ReagentType type, const ReagentInventory& inventory);
	int selectedCount(ReagentType type) const;

	CurseVector curseVector {};
	bool toggledOpen {false};
	bool open {false};
	bool castRequested {false};
	std::string statusLine {"Choose a base, then up to three stacks."};
};
