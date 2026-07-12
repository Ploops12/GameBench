#pragma once

#include <array>
#include <string>
#include "Reagent.hxx"

enum class VectorType {
	Bottle,
	Apple,
	Spider
};

struct SpellStats {
	float damage{0.0f};
	float splashRadius{0.0f};
	float snareSeconds{0.0f};
	float poisonDamage{0.0f};
	float wardGain{0.0f};
	float fearSeconds{0.0f};
	float hazardSeconds{0.0f};
	bool seeks{false};
};

class CurseVector {
public:
	static constexpr int MAX_REAGENTS = 3;

	CurseVector();
	void SetVector(VectorType value) { vector = value; }
	VectorType GetVector() const { return vector; }
	bool AddReagent(ReagentType type, int available);
	void ClearReagents();
	void ConsumeReagents(std::array<int, REAGENT_COUNT>& inventory) const;
	const std::array<ReagentType, MAX_REAGENTS>& GetReagents() const { return reagents; }
	int GetReagentCount() const { return reagentCount; }
	SpellStats CalculateStats() const;
	std::string GetName() const;

private:
	VectorType vector{VectorType::Bottle};
	std::array<ReagentType, MAX_REAGENTS> reagents{};
	int reagentCount{0};
};
