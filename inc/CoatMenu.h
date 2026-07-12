#pragma once

#include "CurseVector.hxx"
#include "InputHandler.hxx"
#include "Menu.hxx"
#include "Player.hxx"

#include <string>

class CoatMenu : public Menu {
public:
	CoatMenu();

	void update(const InputHandler::InputState& input, float dt, const Player& player, CurseVector& spell);
	void draw(const Player& player, const CurseVector& spell) const;
	void draw() const override;
	void reset();
	void clampSelectionToInventory(const Player& player, CurseVector& spell);
	[[nodiscard]] float movementMultiplier() const;
	[[nodiscard]] const std::string& statusMessage() const;

private:
	void setStatus(std::string message);

	const Player* drawPlayer_ {nullptr};
	const CurseVector* drawSpell_ {nullptr};
	std::string statusMessage_;
	float statusTimer_ {0.0f};
};
