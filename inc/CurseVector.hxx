#pragma once

#include <array>
#include <vector>

#include "Reagent.hxx"

enum class BaseVector : int {
	Bottle,
	Apple,
	Spider,
	Poppet
};

struct SpellStats {
	BaseVector vector {BaseVector::Poppet};
	float damage {7.0f};
	float radius {0.32f};
	float poisonDamagePerSecond {};
	float poisonDuration {};
	float snareDuration {};
	float fearDuration {};
	float wardGain {};
	float lingerDuration {};
	float seekRadius {};
	float healthGain {};
};

class CurseVector {
public:
	explicit CurseVector(BaseVector base = BaseVector::Bottle);

	BaseVector getBase() const;
	void setBase(BaseVector value);
	bool addReagent(ReagentType reagent);
	void clearReagents();
	const std::vector<ReagentType>& getReagents() const;
	SpellStats build() const;

	static const char* getName(BaseVector vector);
	static SpellStats build(BaseVector vector, const std::vector<ReagentType>& reagents);

private:
	BaseVector base;
	std::vector<ReagentType> reagents;
};
