#pragma once

#include <raylib.h>

#include <cstddef>

enum class ReagentType {
	GraveSalt,
	SaintAsh,
	BlackWax,
	WidowNettle,
	MothDust,
	ChurchGlass,
	Count
};

constexpr std::size_t ReagentCount = static_cast<std::size_t>(ReagentType::Count);

struct ReagentDefinition {
	const char* name;
	const char* shortName;
	const char* effect;
	Color color;
};

std::size_t reagentIndex(ReagentType type);
const ReagentDefinition& getReagentDefinition(ReagentType type);
ReagentType reagentFromIndex(std::size_t index);

class Reagent {
public:
	explicit Reagent(ReagentType type = ReagentType::GraveSalt, int amount = 1);

	ReagentType getType() const;
	int getAmount() const;
	void addToStack(int amount);
	bool removeFromStack(int amount);

private:
	ReagentType type {ReagentType::GraveSalt};
	int amount {1};
};
