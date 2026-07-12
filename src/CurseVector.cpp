#include "CurseVector.hxx"

#include <algorithm>
#include <sstream>

const char* BaseVectorName(BaseVector vector) {
	switch (vector) {
		case BaseVector::Bottle: return "BOTTLE";
		case BaseVector::Apple: return "APPLE";
		case BaseVector::Spider: return "SPIDER";
		case BaseVector::Poppet: return "POPPET";
	}
	return "POPPET";
}

const char* BaseVectorHint(BaseVector vector) {
	switch (vector) {
		case BaseVector::Bottle: return "throw / impact curse";
		case BaseVector::Apple: return "eat / self-target";
		case BaseVector::Spider: return "place / trap curse";
		case BaseVector::Poppet: return "weak fallback bolt";
	}
	return "weak fallback bolt";
}

CurseVector::CurseVector(BaseVector base) : selectedBase(base) {
}

void CurseVector::setBase(BaseVector base) {
	selectedBase = base;
}

BaseVector CurseVector::base() const {
	return selectedBase;
}

void CurseVector::cycleBase(int direction) {
	constexpr int baseCount = 4;
	int value = static_cast<int>(selectedBase) + (direction < 0 ? -1 : 1);
	value = (value % baseCount + baseCount) % baseCount;
	selectedBase = static_cast<BaseVector>(value);
}

bool CurseVector::addReagent(ReagentType type) {
	if (selectedCount >= static_cast<int>(reagents.size())) return false;
	reagents[selectedCount++] = type;
	return true;
}

void CurseVector::setReagent(int slot, ReagentType type) {
	if (slot < 0 || slot >= static_cast<int>(reagents.size())) return;
	if (slot < selectedCount) {
		reagents[slot] = type;
		return;
	}
	if (slot == selectedCount) {
		reagents[selectedCount++] = type;
	}
}

void CurseVector::removeReagent(int slot) {
	if (slot < 0 || slot >= selectedCount) return;
	for (int i = slot; i < selectedCount - 1; ++i) reagents[i] = reagents[i + 1];
	--selectedCount;
}

void CurseVector::clearReagents() {
	selectedCount = 0;
}

bool CurseVector::contains(ReagentType type) const {
	return countOf(type) > 0;
}

int CurseVector::countOf(ReagentType type) const {
	int count = 0;
	for (int i = 0; i < selectedCount; ++i) {
		if (reagents[i] == type) ++count;
	}
	return count;
}

int CurseVector::reagentCount() const {
	return selectedCount;
}

ReagentType CurseVector::reagentAt(int slot) const {
	if (slot < 0 || slot >= selectedCount) return ReagentType::GraveSalt;
	return reagents[slot];
}

SpellStats CurseVector::stats() const {
	SpellStats result{};
	result.reagentCount = selectedCount;
	result.radius = selectedBase == BaseVector::Spider ? 1.1f : (selectedBase == BaseVector::Poppet ? 0.25f : 0.55f);

	if (selectedBase == BaseVector::Poppet) result.damage = 8;
	if (selectedCount == 0 && (selectedBase == BaseVector::Bottle || selectedBase == BaseVector::Spider)) result.damage = 10;
	for (int i = 0; i < selectedCount; ++i) {
		const ReagentDefinition& reagent = GetReagentDefinition(reagents[i]);
		result.damage += reagent.damage;
		result.wardGain += reagent.wardGain;
		result.poisonPerSecond += reagent.poisonPerSecond;
		result.snareDuration += reagent.snareDuration;
		result.fearDuration += reagent.fearDuration;
		result.radius += reagent.radius;
		result.hazardDuration += reagent.hazardDuration;
		result.seeking = result.seeking || reagent.seeking;
	}
	return result;
}

std::string CurseVector::name() const {
	if (selectedBase == BaseVector::Poppet && selectedCount == 0) return "POPPET SNAP";
	std::string result = BaseVectorName(selectedBase);
	if (selectedCount == 0) return result + " / bare vector";
	result += " / ";
	for (int i = 0; i < selectedCount; ++i) {
		if (i > 0) result += " + ";
		result += GetReagentDefinition(reagents[i]).shortName;
	}
	return result;
}

std::string CurseVector::deliveryText() const {
	return std::string(BaseVectorName(selectedBase)) + ": " + BaseVectorHint(selectedBase);
}

std::string CurseVector::outcomeText() const {
	const SpellStats result = stats();
	std::ostringstream text;
	if (selectedBase == BaseVector::Apple) {
		text << "eat: +" << (12 + result.damage / 2) << " vitality";
		if (result.wardGain > 0.0f) text << "  +" << static_cast<int>(result.wardGain) << " ward";
		if (result.snareDuration > 0.0f) text << "  haste";
		if (result.poisonPerSecond > 0.0f) text << "  rot->regen";
		if (result.fearDuration > 0.0f) text << "  panic ring";
		if (result.seeking) text << "  evasive";
		return text.str();
	}
	text << result.damage << " damage";
	if (result.poisonPerSecond > 0.0f) text << "  rot " << static_cast<int>(result.poisonPerSecond) << "/s";
	if (result.snareDuration > 0.0f) text << "  snare " << static_cast<int>(result.snareDuration * 10.0f) / 10.0f << "s";
	if (result.fearDuration > 0.0f) text << "  panic";
	if (result.radius > 0.6f) text << "  radius " << static_cast<int>(result.radius * 10.0f) / 10.0f;
	if (result.hazardDuration > 0.0f) text << "  linger";
	if (result.seeking) text << "  seeking";
	if (result.wardGain > 0.0f) text << "  +ward";
	return text.str();
}
