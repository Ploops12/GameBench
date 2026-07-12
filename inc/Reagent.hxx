#pragma once

#include <array>
#include <raylib.h>

enum class ReagentType {
	GraveSalt = 0,
	SaintAsh,
	WidowNettle,
	FuneralOil,
	MothDust,
	ChurchGlass,
	Count
};

struct ReagentInfo {
	ReagentType type;
	const char* name;
	const char* shortName;
	const char* effectLabel;
	Color color;
	int keybind;
};

using ReagentInventory = std::array<int, static_cast<int>(ReagentType::Count)>;

const std::array<ReagentInfo, static_cast<int>(ReagentType::Count)>& getReagentInfos();
const ReagentInfo& getReagentInfo(ReagentType type);
int getReagentIndex(ReagentType type);
