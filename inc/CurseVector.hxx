#pragma once

#include <array>
#include <string>
#include "Reagent.hxx"

enum class VectorType { Poppet, Bottle, Apple, Spider };

struct SpellStats {
	float damage{12.0f};
	float radius{0.15f};
	float poisonDamage{0.0f};
	float snareSeconds{0.0f};
	float fearSeconds{0.0f};
	float wardGain{0.0f};
	float duration{0.0f};
	float homing{0.0f};
};

class CurseVector {
public:
	VectorType type{VectorType::Poppet};
	std::array<ReagentType, 3> reagents {};
	int reagentSlots{0};

	SpellStats stats() const;
	const char* name() const;
	std::string ingredients() const;
	std::string preview() const;
};
