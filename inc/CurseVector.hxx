#pragma once

// Curse spell model: one base vector plus zero to three reagent selections, producing predictable composed stats.

#include "Reagent.hxx"

#include <array>
#include <string>
#include <vector>

enum class BaseVector {
	Bottle = 0,
	Apple,
	Spider,
	Poppet,
	Count
};

struct BaseVectorDefinition {
	BaseVector type{};
	const char* name{};
	const char* keyName{};
	const char* deliveryLine{};
	Color color{};
};

struct SpellStats {
	float damage{};
	float radius{};
	float snareSeconds{};
	float rotDps{};
	float rotSeconds{};
	float fearSeconds{};
	float wardGain{};
	float lingerSeconds{};
	float homingStrength{};
	float mineArmingSeconds{};
	float projectileSpeed{};
};

class CurseSpell {
public:
	static constexpr int MAX_REAGENTS = 3;

	void setBase(BaseVector value);
	BaseVector getBase() const;
	bool addReagent(ReagentType type, const ReagentInventory& inventory);
	void clearReagents();
	const std::vector<ReagentType>& getReagents() const;
	bool canAfford(const ReagentInventory& inventory) const;
	void consume(ReagentInventory& inventory) const;
	std::array<int, REAGENT_COUNT> selectedCounts() const;
	SpellStats stats() const;
	std::string previewName() const;
	std::string previewDetails() const;

private:
	BaseVector base{BaseVector::Poppet};
	std::vector<ReagentType> reagents{};
};

const BaseVectorDefinition& baseVectorDefinition(BaseVector type);
BaseVector baseVectorFromIndex(int index);
