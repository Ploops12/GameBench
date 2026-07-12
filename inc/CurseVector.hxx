#pragma once

#include <array>
#include <string>
#include "Reagent.hxx"

enum class BaseVectorType {
	Poppet = 0,
	Bottle,
	Apple,
	Spider
};

struct SpellRecipe {
	BaseVectorType base{BaseVectorType::Poppet};
	std::array<ReagentType, 3> slots{};
	std::array<int, static_cast<int>(ReagentType::Count)> counts{};
	int slotCount{0};

	SpellRecipe();

	int totalSelections() const;
	int getCount(ReagentType type) const;
	bool addReagent(ReagentType type, int availableCount);
	void popSelection();
	void clearReagents();
};

struct ComposedSpell {
	BaseVectorType base{BaseVectorType::Poppet};
	std::array<int, static_cast<int>(ReagentType::Count)> counts{};
	std::string label;
	std::string preview;
	float damage{0.0f};
	float splashRadius{0.35f};
	float snareDuration{0.0f};
	float rotDamagePerSecond{0.0f};
	float rotDuration{0.0f};
	float fearDuration{0.0f};
	float wardGain{0.0f};
	float hazardDuration{0.0f};
	float homingStrength{0.0f};
	float projectileSpeed{0.0f};
	float lifetime{0.0f};
	float triggerRadius{0.0f};
	bool fallbackPoppet{false};
};

const char* getBaseVectorName(BaseVectorType base);
Color getBaseVectorColor(BaseVectorType base);
ComposedSpell composeSpell(const SpellRecipe& recipe);
