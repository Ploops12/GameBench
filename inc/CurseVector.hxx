#pragma once

#include <array>
#include <string>
#include "Reagent.hxx"

enum class BaseVector : int {
	Bottle,
	Apple,
	Spider,
	Poppet
};

struct SpellStats {
	float damage {};
	float splashRadius {};
	float snareDuration {};
	float rotDamagePerSecond {};
	float fearDuration {};
	float wardGain {};
	float lingeringDuration {};
	float healAmount {};
	int homingStrength {};
};

class CurseVector {
public:
	explicit CurseVector(BaseVector base = BaseVector::Poppet);

	void setBase(BaseVector value) { base = value; }
	BaseVector getBase() const { return base; }
	bool addReagent(ReagentType reagent);
	void clearReagents();
	int getReagentCount() const { return reagentCount; }
	const std::array<ReagentType, 3>& getReagents() const { return reagents; }
	SpellStats compose() const;
	std::string preview() const;

	static const char* baseName(BaseVector value);
private:
	BaseVector base {BaseVector::Poppet};
	std::array<ReagentType, 3> reagents {};
	int reagentCount {};
};
