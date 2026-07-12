#pragma once

#include <array>
#include <cstddef>
#include <raylib.h>

enum class ReagentType : std::size_t {
	BoneShards,
	WidowNettle,
	Rotcap,
	BatWing,
	WitchGlass,
	PreservedEye,
	Count
};

constexpr std::size_t REAGENT_TYPE_COUNT = static_cast<std::size_t>(ReagentType::Count);

struct ReagentDefinition {
	ReagentType type;
	const char* name;
	const char* shortName;
	const char* effect;
	int selectionKey;
	Color color;
};

const std::array<ReagentDefinition, REAGENT_TYPE_COUNT>& getReagentDefinitions();
const ReagentDefinition& getReagentDefinition(ReagentType type);

using ReagentInventory = std::array<int, REAGENT_TYPE_COUNT>;

class Reagent {
	// Stackable value used by pickups, inventory, and the coat menu.
public:
	Reagent(ReagentType type = ReagentType::BoneShards, int amount = 1);

	ReagentType getType() const;
	int getAmount() const;
	int take(int requestedAmount);

private:
	ReagentType type;
	int amount;
};
