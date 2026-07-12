#include "Reagent.hxx"

#include <array>

const std::array<Reagent, REAGENT_COUNT>& getReagents() {
	static const std::array<Reagent, REAGENT_COUNT> reagents {{
		{ReagentType::GraveSalt, "Grave Salt", "+8 hex damage", {232, 232, 208, 255}},
		{ReagentType::WidowNettle, "Widow Nettle", "thorn snare +1.4s", {99, 214, 115, 255}},
		{ReagentType::PlagueHoney, "Plague Honey", "rot +5/s for 3s", {226, 183, 64, 255}},
		{ReagentType::MothDust, "Moth Dust", "panic + seeking", {174, 132, 239, 255}},
		{ReagentType::SaintAsh, "Saint Ash", "+18 ward on cast", {111, 216, 237, 255}},
		{ReagentType::BlackWax, "Black Wax", "lingering blight +2s", {229, 91, 108, 255}}
	}};
	return reagents;
}

const Reagent& getReagent(ReagentType type) {
	return getReagents()[static_cast<int>(type)];
}
