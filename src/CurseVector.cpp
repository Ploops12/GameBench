#include "CurseVector.hxx"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace {

std::string wholeNumber(float value) {
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(value >= 10.0f ? 0 : 1) << value;
	return stream.str();
}

}

CurseVector::CurseVector(CurseVectorType initialBase)
	: base(initialBase) {
}

void CurseVector::setBase(CurseVectorType newBase) {
	base = newBase;
}

CurseVectorType CurseVector::getBase() const {
	return base;
}

bool CurseVector::tryAddReagent(ReagentType type, int availableAmount) {
	if (reagentCount >= MaxReagents || availableAmount <= selectedAmount(type)) {
		return false;
	}

	reagents[static_cast<std::size_t>(reagentCount)] = type;
	++reagentCount;
	return true;
}

bool CurseVector::removeLastReagent() {
	if (reagentCount == 0) {
		return false;
	}

	--reagentCount;
	reagents[static_cast<std::size_t>(reagentCount)] = ReagentType::Count;
	return true;
}

void CurseVector::clearReagents() {
	for (ReagentType& reagent : reagents) {
		reagent = ReagentType::Count;
	}
	reagentCount = 0;
}

int CurseVector::getReagentCount() const {
	return reagentCount;
}

const std::array<ReagentType, CurseVector::MaxReagents>& CurseVector::getReagents() const {
	return reagents;
}

SpellStats CurseVector::getStats() const {
	SpellStats stats {};

	switch (base) {
	case CurseVectorType::Bottle:
		stats.damage = 12.0f;
		stats.splashRadius = 2.25f;
		stats.projectileSpeed = 19.0f;
		stats.castCooldown = 0.42f;
		stats.color = {121, 212, 255, 255};
		break;
	case CurseVectorType::Apple:
		stats.heal = 16.0f;
		stats.wardGain = 8.0f;
		stats.splashRadius = 2.0f;
		stats.castCooldown = 0.65f;
		stats.color = {244, 104, 107, 255};
		break;
	case CurseVectorType::Spider:
		stats.damage = 10.0f;
		stats.splashRadius = 2.6f;
		stats.mineDuration = 11.0f;
		stats.castCooldown = 0.55f;
		stats.color = {154, 113, 255, 255};
		break;
	case CurseVectorType::Poppet:
		stats.damage = 6.0f;
		stats.splashRadius = 0.35f;
		stats.seekingStrength = 0.55f;
		stats.castCooldown = 0.22f;
		stats.color = {246, 224, 164, 255};
		break;
	}

	for (int index = 0; index < reagentCount; ++index) {
		switch (reagents[static_cast<std::size_t>(index)]) {
		case ReagentType::GraveSalt:
			stats.damage += 7.0f;
			stats.heal += 7.0f;
			break;
		case ReagentType::SaintAsh:
			stats.wardGain += 15.0f;
			break;
		case ReagentType::BlackWax:
			stats.hazardDuration += 2.0f;
			stats.splashRadius += 0.35f;
			break;
		case ReagentType::WidowNettle:
			stats.snareDuration += 1.4f;
			break;
		case ReagentType::MothDust:
			stats.fearDuration += 1.2f;
			stats.seekingStrength += 0.3f;
			break;
		case ReagentType::ChurchGlass:
			stats.rotDamagePerSecond += 2.5f;
			stats.rotDuration += 1.25f;
			break;
		case ReagentType::Count:
			break;
		}
	}

	return stats;
}

std::vector<std::string> CurseVector::getPreviewLines() const {
	const SpellStats stats = getStats();
	std::vector<std::string> lines;
	lines.emplace_back(std::string("DELIVERY: ") + getDeliveryName(base));

	if (base == CurseVectorType::Apple) {
		lines.emplace_back("EATEN: +" + wholeNumber(stats.heal) + " health  +" + wholeNumber(stats.wardGain) + " ward");
		lines.emplace_back("CLOSE PULSE: " + wholeNumber(stats.damage) + " harm / " + wholeNumber(stats.splashRadius) + "m");
	} else if (base == CurseVectorType::Spider) {
		lines.emplace_back("MINE: " + wholeNumber(stats.damage) + " harm / " + wholeNumber(stats.splashRadius) + "m");
		lines.emplace_back("LIVES: " + wholeNumber(stats.mineDuration) + "s before withering");
	} else if (base == CurseVectorType::Bottle) {
		lines.emplace_back("HARM: " + wholeNumber(stats.damage) + "  BLAST: " + wholeNumber(stats.splashRadius) + "m");
		lines.emplace_back("THROW: " + wholeNumber(stats.projectileSpeed) + "m/s");
	} else {
		lines.emplace_back("NEEDLE: " + wholeNumber(stats.damage) + " harm  /  quick, no reagent cost");
	}

	if (stats.snareDuration > 0.0f) {
		lines.emplace_back("THORNS: snare " + wholeNumber(stats.snareDuration) + "s");
	}
	if (stats.rotDuration > 0.0f) {
		lines.emplace_back("ROT: " + wholeNumber(stats.rotDamagePerSecond) + "/s for " + wholeNumber(stats.rotDuration) + "s");
	}
	if (stats.fearDuration > 0.0f) {
		lines.emplace_back("PANIC: " + wholeNumber(stats.fearDuration) + "s" + (stats.seekingStrength > 0.0f ? " / seeks prey" : ""));
	}
	if (stats.hazardDuration > 0.0f) {
		lines.emplace_back("BLACK WAX: leaves a " + wholeNumber(stats.hazardDuration) + "s blight");
	}

	return lines;
}

std::string CurseVector::getDisplayName() const {
	std::string name = getBaseName(base);
	for (int index = 0; index < reagentCount; ++index) {
		name += index == 0 ? " + " : ", ";
		name += getReagentDefinition(reagents[static_cast<std::size_t>(index)]).shortName;
	}
	return name;
}

const char* CurseVector::getBaseName(CurseVectorType type) {
	switch (type) {
	case CurseVectorType::Bottle:
		return "HEX BOTTLE";
	case CurseVectorType::Apple:
		return "BITTER APPLE";
	case CurseVectorType::Spider:
		return "WIDOW SPIDER";
	case CurseVectorType::Poppet:
		return "NEEDLE POPPET";
	}
	return "UNKNOWN HEX";
}

const char* CurseVector::getDeliveryName(CurseVectorType type) {
	switch (type) {
	case CurseVectorType::Bottle:
		return "THROWN PROJECTILE";
	case CurseVectorType::Apple:
		return "EATEN SELF-PULSE";
	case CurseVectorType::Spider:
		return "PLACED MINE";
	case CurseVectorType::Poppet:
		return "DIRECTED NEEDLE";
	}
	return "UNKNOWN";
}

int CurseVector::selectedAmount(ReagentType type) const {
	return static_cast<int>(std::count(reagents.begin(), reagents.begin() + reagentCount, type));
}
