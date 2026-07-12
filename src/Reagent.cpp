#include "Reagent.hxx"

const std::array<ReagentInfo, static_cast<int>(ReagentType::Count)>& getReagentInfos() {
	static const std::array<ReagentInfo, static_cast<int>(ReagentType::Count)> info {{
		{ReagentType::GraveSalt, "Grave Salt", "salt", "damage", {236, 236, 236, 255}, KEY_Q},
		{ReagentType::SaintAsh, "Saint Ash", "ash", "ward siphon", {255, 221, 150, 255}, KEY_E},
		{ReagentType::WidowNettle, "Widow Nettle", "nettle", "thorn snare", {140, 223, 132, 255}, KEY_R},
		{ReagentType::FuneralOil, "Funeral Oil", "oil", "rot haze", {121, 201, 117, 255}, KEY_F},
		{ReagentType::MothDust, "Moth Dust", "moth", "fear / seeking", {186, 151, 255, 255}, KEY_Z},
		{ReagentType::ChurchGlass, "Church Glass", "glass", "splash", {120, 220, 255, 255}, KEY_X}
	}};
	return info;
}

const ReagentInfo& getReagentInfo(ReagentType type) {
	return getReagentInfos()[getReagentIndex(type)];
}

int getReagentIndex(ReagentType type) {
	return static_cast<int>(type);
}
