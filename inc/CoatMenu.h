#pragma once

#include <array>
#include "CurseVector.hxx"

class CoatMenu {
public:
	void update(std::array<int, static_cast<int>(ReagentType::Count)>& inventory);
	void draw(int screenWidth, int screenHeight, const std::array<int, static_cast<int>(ReagentType::Count)>& inventory, float movementPenalty) const;
	bool isOpen() const { return open; }
	const CurseVector& spell() const { return selected; }
	void clearSpell();
private:
	bool open{};
	CurseVector selected{};
};
