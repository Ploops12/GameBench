#pragma once

#include <array>
#include <raylib.h>

enum class ReagentType { GraveSalt, WidowNettle, PlagueHoney, MothDust, SaintAsh, RavenEye, Count };

struct ReagentInfo {
	const char* name;
	const char* effect;
	Color color;
};

const ReagentInfo& GetReagentInfo(ReagentType type);
constexpr int ReagentCount = static_cast<int>(ReagentType::Count);
using ReagentInventory = std::array<int, ReagentCount>;
