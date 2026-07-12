#pragma once

#include <array>
#include <string>

#include "Reagent.hxx"

enum class CurseVectorType {
	Bottle,
	Apple,
	Spider,
	Poppet
};

struct SpellStats {
	float impactDamage {0.0f};
	float radius {0.0f};
	float snareDuration {0.0f};
	float rotDamagePerSecond {0.0f};
	float rotDuration {0.0f};
	float fearDuration {0.0f};
	float wardGain {0.0f};
	float seekingStrength {0.0f};
	float lingeringDuration {0.0f};
	float selfHeal {0.0f};
	float hasteDuration {0.0f};
	float cooldown {0.35f};
	float projectileSpeed {0.0f};
};

class CurseVector {
	// Exactly one delivery vector and up to three additive reagent doses.
public:
	static constexpr int MAX_REAGENTS = 3;

	explicit CurseVector(CurseVectorType type = CurseVectorType::Poppet);

	CurseVectorType getType() const;
	void setType(CurseVectorType value);
	bool addReagent(ReagentType reagent, const ReagentInventory& inventory);
	bool removeLastReagent();
	bool removeReagent(ReagentType reagent);
	void clearReagents();
	int getReagentCount() const;
	ReagentType getReagent(int index) const;
	int getDoseCount(ReagentType reagent) const;
	bool canAfford(const ReagentInventory& inventory) const;
	bool consume(ReagentInventory& inventory) const;

	SpellStats calculateStats() const;
	std::string getVectorName() const;
	std::string getCurseName() const;
	std::string getPreview() const;

private:
	CurseVectorType type;
	std::array<ReagentType, MAX_REAGENTS> reagents {};
	int reagentCount {0};
};
