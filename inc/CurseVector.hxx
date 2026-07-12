#pragma once

#include <array>
#include <string>
#include <vector>
#include "Reagent.hxx"

enum class BaseVectorType {
	Poppet = 0,
	Bottle,
	Apple,
	Spider,
	Count
};

struct SpellPreview {
	BaseVectorType base {BaseVectorType::Poppet};
	std::array<int, ReagentTypeCount> stacks {};
	int totalSelections {0};
	bool fallbackPoppet {false};
	float damage {0.0f};
	float radius {0.0f};
	float rotDamagePerSecond {0.0f};
	float rotDuration {0.0f};
	float snareDuration {0.0f};
	float fearDuration {0.0f};
	float seekStrength {0.0f};
	float wardGain {0.0f};
	float lingerDuration {0.0f};
	float cooldown {0.25f};
	float projectileSpeed {0.0f};
	float lifetime {0.0f};
};

class CurseVector {
public:
	CurseVector();

	void setBase(BaseVectorType value);
	BaseVectorType getBase() const;

	bool addReagent(ReagentType type, const std::array<int, ReagentTypeCount>& inventory);
	bool removeLast();
	void clear();
	void pruneToInventory(const std::array<int, ReagentTypeCount>& inventory);

	int selectionCount() const;
	const std::vector<ReagentType>& getSelections() const;
	std::array<int, ReagentTypeCount> getStacks() const;

	SpellPreview buildPreview(bool allowFallbackPoppet = true) const;
private:
	BaseVectorType base {BaseVectorType::Poppet};
	std::vector<ReagentType> selections;
};

const char* GetBaseVectorName(BaseVectorType base);
const char* GetBaseVectorShortName(BaseVectorType base);
std::vector<std::string> DescribeSpell(const SpellPreview& preview);
