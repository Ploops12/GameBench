#pragma once

#include <array>
#include "Reagent.hxx"

enum class CurseBase { Poppet, Bottle, Apple, Spider };

struct CurseVector {
	CurseBase base{CurseBase::Poppet};
	std::array<ReagentType, 3> reagents{};
	int reagentCount{};
	float damage{12.0f};
	float radius{0.0f};
	float duration{0.0f};
	bool seeking{};
	bool snare{};
	bool poison{};
	bool fear{};
	bool wardGain{};

	void rebuild();
	const char* baseName() const;
	const char* deliveryName() const;
};
