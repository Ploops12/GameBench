#pragma once

#include "CurseVector.hxx"
#include "InputHandler.hxx"
#include "Menu.hxx"

class Player;

class CoatMenu : public Menu {
public:
	CoatMenu();

	void update(float deltaTime) override;
	void draw() const override;
	void update(const InputHandler::InputState& input, const Player& player);
	void draw(const Player& player, int screenWidth, int screenHeight) const;

	void open();
	void close();
	bool isOpen() const;

	bool selectReagent(ReagentType type, const Player& player);
	void clearSelection();
	void selectBase(BaseVector base);
	void cycleBase(int direction);
	const CurseVector& spell() const;

private:
	void selectAtMouse(Vector2 mouse, const Player& player, int screenWidth, int screenHeight);

	CurseVector currentSpell{};
	int activeSlot{0};
};
