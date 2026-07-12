#pragma once

#include <array>
#include <raylib.h>
#include "CurseVector.hxx"

class CoatMenu {
public:
	bool IsOpen() const { return open; }
	void Toggle();
	void Close() { open = false; }
	void Update(CurseVector& curse, const std::array<int, REAGENT_COUNT>& inventory);
	void Draw(const std::array<int, REAGENT_COUNT>& inventory, const CurseVector& curse) const;
	void SelectVector(CurseVector& curse, VectorType vector) const;
	bool AddReagent(CurseVector& curse, ReagentType type, const std::array<int, REAGENT_COUNT>& inventory) const;

private:
	bool open{false};
};
