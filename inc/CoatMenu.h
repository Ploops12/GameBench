#pragma once

#include <array>
#include <vector>

#include "CurseVector.hxx"

class CoatMenu {
public:
	void setOpen(bool value);
	bool isOpen() const;
	void update(const std::array<int, REAGENT_COUNT>& inventory);
	void draw(const std::array<int, REAGENT_COUNT>& inventory) const;
	BaseVector getBase() const;
	const std::vector<ReagentType>& getSelected() const;
	SpellStats getPreview() const;
	bool consume(std::array<int, REAGENT_COUNT>& inventory);

private:
	bool canAdd(ReagentType reagent, const std::array<int, REAGENT_COUNT>& inventory) const;
	bool open {};
	BaseVector base {BaseVector::Bottle};
	std::vector<ReagentType> selected;
	int highlighted {};
};
