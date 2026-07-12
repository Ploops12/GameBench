#pragma once

#include "Reagent.hxx"

#include <array>
#include <string>

enum class BaseVector : int {
	Poppet = 0,
	Bottle,
	Apple,
	Spider
};

struct SpellStats {
	int damage{0};
	float wardGain{0.0f};
	float poisonPerSecond{0.0f};
	float snareDuration{0.0f};
	float fearDuration{0.0f};
	float radius{0.0f};
	float hazardDuration{0.0f};
	bool seeking{false};
	int reagentCount{0};
};

const char* BaseVectorName(BaseVector vector);
const char* BaseVectorHint(BaseVector vector);

class CurseVector {
public:
	explicit CurseVector(BaseVector base = BaseVector::Poppet);

	void setBase(BaseVector base);
	BaseVector base() const;
	void cycleBase(int direction);

	bool addReagent(ReagentType type);
	void setReagent(int slot, ReagentType type);
	void removeReagent(int slot);
	void clearReagents();
	bool contains(ReagentType type) const;
	int countOf(ReagentType type) const;
	int reagentCount() const;
	ReagentType reagentAt(int slot) const;

	SpellStats stats() const;
	std::string name() const;
	std::string deliveryText() const;
	std::string outcomeText() const;

private:
	BaseVector selectedBase{BaseVector::Poppet};
	std::array<ReagentType, 3> reagents{};
	int selectedCount{0};
};
