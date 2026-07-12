#pragma once

#include <array>
#include <raylib.h>
#include <string_view>

enum class ReagentType {
	GraveSalt = 0,
	WidowNettle,
	MothDust,
	BlackWax,
	SaintAsh,
	BellFilings,
	Count
};

struct ReagentDefinition {
	ReagentType type{};
	std::string_view name;
	std::string_view shortName;
	std::string_view effect;
	Color color{};
};

constexpr int REAGENT_COUNT = static_cast<int>(ReagentType::Count);

inline const std::array<ReagentDefinition, REAGENT_COUNT>& GetReagentDefinitions() {
	static const std::array<ReagentDefinition, REAGENT_COUNT> definitions{{
		{ReagentType::GraveSalt, "Grave Salt", "SALT", "more curse damage", Color{220, 225, 210, 255}},
		{ReagentType::WidowNettle, "Widow Nettle", "NETTLE", "thorn snare and rot", Color{68, 220, 92, 255}},
		{ReagentType::MothDust, "Moth Dust", "MOTH", "fear and panic", Color{235, 210, 118, 255}},
		{ReagentType::BlackWax, "Black Wax", "WAX", "lingering hex pools", Color{38, 38, 48, 255}},
		{ReagentType::SaintAsh, "Saint Ash", "ASH", "ward gain", Color{178, 190, 210, 255}},
		{ReagentType::BellFilings, "Bell Filings", "BELL", "seeking bell-hunger", Color{232, 170, 76, 255}},
	}};

	return definitions;
}

inline const ReagentDefinition& GetReagentDefinition(ReagentType type) {
	return GetReagentDefinitions()[static_cast<int>(type)];
}
