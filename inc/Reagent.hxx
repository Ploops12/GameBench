#pragma once

#include <cstddef>
#include <raylib.h>

enum class ReagentType {
	GraveSalt = 0,
	SaintAsh,
	WidowNettle,
	BellBronze,
	MothDust,
	BogRot,
	Count
};

constexpr int ReagentTypeCount = static_cast<int>(ReagentType::Count);

struct ReagentDescriptor {
	ReagentType type;
	const char* name;
	const char* shortName;
	const char* effectLabel;
	Color color;
	int key;
};

class Reagent {
public:
	explicit Reagent(ReagentType type = ReagentType::GraveSalt);

	ReagentType getType() const;
	const ReagentDescriptor& descriptor() const;
private:
	ReagentType type {ReagentType::GraveSalt};
};

const ReagentDescriptor& GetReagentDescriptor(ReagentType type);
const ReagentDescriptor& GetReagentDescriptor(std::size_t index);
