#include "CurseVector.hxx"

#include <algorithm>

CurseVector::CurseVector(BaseVector value) : base(value) {}

BaseVector CurseVector::getBase() const {
	return base;
}

void CurseVector::setBase(BaseVector value) {
	base = value;
}

bool CurseVector::addReagent(ReagentType reagent) {
	if (reagents.size() == 3) return false;
	reagents.push_back(reagent);
	return true;
}

void CurseVector::clearReagents() {
	reagents.clear();
}

const std::vector<ReagentType>& CurseVector::getReagents() const {
	return reagents;
}

SpellStats CurseVector::build() const {
	return build(base, reagents);
}

const char* CurseVector::getName(BaseVector vector) {
	switch (vector) {
	case BaseVector::Bottle: return "Bottle";
	case BaseVector::Apple: return "Apple";
	case BaseVector::Spider: return "Spider";
	case BaseVector::Poppet: return "Poppet";
	}
	return "Poppet";
}

SpellStats CurseVector::build(BaseVector vector, const std::vector<ReagentType>& reagents) {
	SpellStats result;
	result.vector = vector;

	switch (vector) {
	case BaseVector::Bottle:
		result.damage = 15.0f;
		result.radius = 1.0f;
		break;
	case BaseVector::Apple:
		result.healthGain = 12.0f;
		result.radius = 1.15f;
		break;
	case BaseVector::Spider:
		result.damage = 12.0f;
		result.radius = 1.35f;
		break;
	case BaseVector::Poppet:
		result.damage = 7.0f;
		result.radius = 0.32f;
		break;
	}

	for (ReagentType reagent : reagents) {
		switch (reagent) {
		case ReagentType::GraveSalt: result.damage += 8.0f; break;
		case ReagentType::WidowNettle: result.snareDuration += 1.4f; break;
		case ReagentType::PlagueHoney:
			result.poisonDamagePerSecond += 5.0f;
			result.poisonDuration += 3.0f;
			break;
		case ReagentType::MothDust:
			result.fearDuration += 1.8f;
			result.seekRadius += 6.0f;
			break;
		case ReagentType::SaintAsh: result.wardGain += 18.0f; break;
		case ReagentType::BlackWax:
			result.lingerDuration += 2.0f;
			result.radius += 0.35f;
			break;
		case ReagentType::Count: break;
		}
	}

	return result;
}
