#pragma once

#include <array>

#include <raylib.h>

enum class ReagentType : int {
	GraveSalt,
	WidowNettle,
	PlagueHoney,
	MothDust,
	SaintAsh,
	BlackWax,
	Count
};

constexpr int REAGENT_COUNT = static_cast<int>(ReagentType::Count);

struct Reagent {
	ReagentType type {};
	const char* name {};
	const char* shortEffect {};
	Color color {};
};

const Reagent& getReagent(ReagentType type);
const std::array<Reagent, REAGENT_COUNT>& getReagents();
