#pragma once

#include "Reagent.hxx"

#include <cstddef>
#include <string>
#include <vector>

enum class BaseVectorType {
	Bottle,
	Apple,
	Spider,
	Poppet
};

struct SpellStats {
	float damage {0.0f};
	float radius {0.0f};
	float poisonDps {0.0f};
	float snareStrength {0.0f};
	float fearDuration {0.0f};
	float wardGain {0.0f};
	float duration {0.0f};
	float heal {0.0f};
	float projectileSpeed {0.0f};
	float cooldown {0.35f};
	float homingStrength {0.0f};
	bool lingering {false};
};

class CurseVector {
public:
	static constexpr std::size_t MAX_REAGENTS = 3;

	CurseVector();

	void selectBase(BaseVectorType base);
	[[nodiscard]] BaseVectorType base() const;
	[[nodiscard]] bool addReagent(ReagentType type);
	[[nodiscard]] bool removeLastReagent();
	void clearReagents();
	[[nodiscard]] const std::vector<ReagentType>& reagents() const;
	[[nodiscard]] int reagentCount(ReagentType type) const;
	[[nodiscard]] bool isFallback() const;

	[[nodiscard]] SpellStats stats() const;
	[[nodiscard]] std::string name() const;
	[[nodiscard]] std::string baseName() const;
	[[nodiscard]] std::string ingredientSummary() const;
	[[nodiscard]] bool canConsume(const ReagentInventory& inventory) const;

private:
	BaseVectorType base_ {BaseVectorType::Poppet};
	std::vector<ReagentType> reagents_;
};
