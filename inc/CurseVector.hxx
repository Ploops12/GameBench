#pragma once

#include "Reagent.hxx"
#include <array>
#include <string>
#include <string_view>

enum class BaseVector {
	Poppet = 0,
	Bottle,
	Apple,
	Spider,
	Count
};

struct BaseVectorDefinition {
	BaseVector type{};
	std::string_view name;
	std::string_view delivery;
	std::string_view keyHint;
};

struct SpellStats {
	float damage{12.0f};
	float radius{0.8f};
	float speed{16.0f};
	float duration{0.0f};
	float poisonDps{0.0f};
	float snareSeconds{0.0f};
	float fearSeconds{0.0f};
	float wardGain{0.0f};
	float seekStrength{0.0f};
	bool lingering{false};
};

struct SpellDraft {
	BaseVector base{BaseVector::Poppet};
	std::array<ReagentType, 3> reagents{};
	int reagentCount{0};
};

inline const std::array<BaseVectorDefinition, static_cast<int>(BaseVector::Count)>& GetBaseVectorDefinitions() {
	static const std::array<BaseVectorDefinition, static_cast<int>(BaseVector::Count)> definitions{{
		{BaseVector::Poppet, "Poppet", "weak hand-cast jab", "TAB"},
		{BaseVector::Bottle, "Bottle", "thrown shattering curse", "1"},
		{BaseVector::Apple, "Apple", "bitten self-hex", "2"},
		{BaseVector::Spider, "Spider", "placed skittering mine", "3"},
	}};

	return definitions;
}

inline const BaseVectorDefinition& GetBaseVectorDefinition(BaseVector type) {
	return GetBaseVectorDefinitions()[static_cast<int>(type)];
}

SpellStats CompileSpell(const SpellDraft& draft);
std::string BuildSpellPreview(const SpellDraft& draft);
