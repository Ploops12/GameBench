#pragma once

#include <array>
#include <string>
#include <raylib.h>
#include "Object.hxx"

enum class ReagentType {
	GraveSalt,
	SaintAsh,
	BlackWax,
	WidowNettle,
	BellBronze,
	MothDust,
	Count
};

constexpr int REAGENT_COUNT = static_cast<int>(ReagentType::Count);

const char* ReagentName(ReagentType type);
Color ReagentColor(ReagentType type);
const char* ReagentKey(ReagentType type);

class Reagent : public Object {
public:
	Reagent(ReagentType type = ReagentType::GraveSalt, int amount = 1);

	ReagentType GetType() const { return type; }
	int GetAmount() const { return amount; }
	void SetAmount(int value) { amount = value; }
	void DrawWorld() const;

private:
	ReagentType type;
	int amount;
};
