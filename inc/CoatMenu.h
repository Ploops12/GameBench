#pragma once

#include "CurseVector.hxx"
#include "Menu.hxx"
#include "InputHandler.hxx"

class CoatMenu : public Menu {
public:
	CoatMenu();

	void update(const InputHandler::InputState& input, const ReagentInventory& inventory);
	void draw(const ReagentInventory& inventory) const;

	const SpellRecipe& getRecipe() const;
	ComposedSpell getPreview() const;
	float getMovementScale() const;

private:
	SpellRecipe recipe;
};
