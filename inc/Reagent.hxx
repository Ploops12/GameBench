#pragma once

// Stackable reagent definitions and inventory helpers for curse composition.

#include <array>
#include <string>
#include <raylib.h>

enum class ReagentType {
	GraveSalt = 0,
	SaintAsh,
	BlackWax,
	WidowNettle,
	MothDust,
	BellBronze,
	Count
};

constexpr int REAGENT_COUNT = static_cast<int>(ReagentType::Count);
using ReagentInventory = std::array<int, REAGENT_COUNT>;

struct ReagentDefinition {
	ReagentType type{};
	const char* name{};
	const char* shortName{};
	const char* keyName{};
	const char* effectLine{};
	Color color{};
};

const ReagentDefinition& reagentDefinition(ReagentType type);
ReagentType reagentFromIndex(int index);
int reagentIndex(ReagentType type);
std::string reagentListText(const std::array<int, REAGENT_COUNT>& counts);
