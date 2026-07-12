#pragma once

#include <array>
#include <raylib.h>

enum class ReagentType : int {
	GraveSalt,
	WidowNettle,
	BogMarrow,
	MothDust,
	SaintAsh,
	ThornHeart,
	Count
};

constexpr int REAGENT_COUNT = static_cast<int>(ReagentType::Count);

const char* reagentName(ReagentType type);
const char* reagentShortEffect(ReagentType type);
Color reagentColor(ReagentType type);
