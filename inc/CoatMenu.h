#pragma once

#include "CurseVector.hxx"
#include "InputHandler.hxx"
#include "Menu.hxx"

#include <array>
#include <string>

struct CoatMenuInputResult {
	bool consumedMouseClick {false};
	bool castRequested {false};
};

class CoatMenu : public Menu {
public:
	CoatMenu();

	CoatMenuInputResult update(const InputHandler::InputState& input,
		const std::array<int, ReagentCount>& inventory, int screenWidth, int screenHeight);
	void draw(int screenWidth, int screenHeight) const override;
	const CurseVector& getSpell() const;
	CurseVector& getSpell();
	const std::string& getStatusText() const;

private:
	void chooseBase(CurseVectorType type);
	void chooseReagent(ReagentType type, int availableAmount);

	CurseVector spell {};
	std::array<int, ReagentCount> displayedInventory {};
	bool toggledOpen {false};
	std::string statusText {"Hold R to rummage.  The hunt does not stop."};
};
