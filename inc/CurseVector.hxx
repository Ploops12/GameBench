#pragma once

#include <array>
#include <string>
#include "Reagent.hxx"

enum class BaseVector { Bottle, Apple, Spider, Poppet };

struct SpellStats {
	int damage{8};
	float radius{0.0f};
	float duration{0.0f};
	float snare{0.0f};
	float seeking{0.0f};
	int wardGain{0};
	bool panic{false};
	bool rot{false};
	std::string effects{"weak poppet sting"};
};

class CurseVector {
public:
	explicit CurseVector(BaseVector base = BaseVector::Bottle) : base(base) {}
	void setBase(BaseVector value) { base = value; }
	BaseVector getBase() const { return base; }
	void clearReagents() { reagents.fill(-1); }
	bool addReagent(ReagentType type);
	void removeLastReagent();
	const std::array<int, 3> &getReagents() const { return reagents; }
	SpellStats stats() const;
	std::string preview() const;

private:
	BaseVector base;
	std::array<int, 3> reagents{{-1, -1, -1}};
};
