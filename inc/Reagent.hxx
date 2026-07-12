#pragma once

#include <array>
#include <string>
#include <raylib.h>

enum class ReagentType { GraveSalt, SaintAsh, BlackWax, WidowNettle, MothDust, BogMarrow, Count };

struct ReagentDefinition {
	const char *name;
	const char *shortName;
	const char *effect;
	Color color;
	int maxStack;
};

const ReagentDefinition &getReagentDefinition(ReagentType type);
constexpr int REAGENT_COUNT = static_cast<int>(ReagentType::Count);

class Reagent {
public:
	Reagent(ReagentType type = ReagentType::GraveSalt, int amount = 1) : type(type), amount(amount) {}
	ReagentType getType() const { return type; }
	int getAmount() const { return amount; }

private:
	ReagentType type;
	int amount;
};
