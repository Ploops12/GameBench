#pragma once

#include "Reagent.hxx"

#include <array>
#include <string>
#include <vector>

enum class CurseVectorType {
	Bottle,
	Apple,
	Spider,
	Poppet
};

struct SpellStats {
	float damage {0.0f};
	float heal {0.0f};
	float splashRadius {0.0f};
	float projectileSpeed {0.0f};
	float mineDuration {0.0f};
	float snareDuration {0.0f};
	float rotDuration {0.0f};
	float rotDamagePerSecond {0.0f};
	float fearDuration {0.0f};
	float wardGain {0.0f};
	float hazardDuration {0.0f};
	float seekingStrength {0.0f};
	float castCooldown {0.25f};
	Color color {MAGENTA};
};

class CurseVector {
public:
	static constexpr int MaxReagents = 3;

	explicit CurseVector(CurseVectorType base = CurseVectorType::Poppet);

	void setBase(CurseVectorType newBase);
	CurseVectorType getBase() const;
	bool tryAddReagent(ReagentType type, int availableAmount);
	bool removeLastReagent();
	void clearReagents();
	int getReagentCount() const;
	const std::array<ReagentType, MaxReagents>& getReagents() const;
	SpellStats getStats() const;
	std::vector<std::string> getPreviewLines() const;
	std::string getDisplayName() const;

	static const char* getBaseName(CurseVectorType type);
	static const char* getDeliveryName(CurseVectorType type);

private:
	int selectedAmount(ReagentType type) const;

	CurseVectorType base {CurseVectorType::Poppet};
	std::array<ReagentType, MaxReagents> reagents {
		ReagentType::Count,
		ReagentType::Count,
		ReagentType::Count
	};
	int reagentCount {0};
};
