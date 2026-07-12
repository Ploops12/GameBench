#include "CurseVector.hxx"

#include <sstream>

CurseVector::CurseVector() {
	ClearReagents();
}

bool CurseVector::AddReagent(ReagentType type, int available) {
	if (reagentCount >= MAX_REAGENTS || available <= 0) return false;
	int selected = 0;
	for (int i = 0; i < reagentCount; ++i) {
		if (reagents[i] == type) ++selected;
	}
	if (selected >= available) return false;
	reagents[reagentCount++] = type;
	return true;
}

void CurseVector::ClearReagents() {
	reagentCount = 0;
	for (auto& reagent : reagents) reagent = ReagentType::GraveSalt;
}

void CurseVector::ConsumeReagents(std::array<int, REAGENT_COUNT>& inventory) const {
	for (int i = 0; i < reagentCount; ++i) {
		const int index = static_cast<int>(reagents[i]);
		if (index >= 0 && index < REAGENT_COUNT && inventory[index] > 0) --inventory[index];
	}
}

SpellStats CurseVector::CalculateStats() const {
	SpellStats stats{};
	switch (vector) {
		case VectorType::Bottle:
			stats.damage = 10.0f;
			break;
		case VectorType::Apple:
			stats.wardGain = 8.0f;
			break;
		case VectorType::Spider:
			stats.damage = 8.0f;
			stats.splashRadius = 1.15f;
			stats.hazardSeconds = 10.0f;
			break;
	}
	for (int i = 0; i < reagentCount; ++i) {
		switch (reagents[i]) {
			case ReagentType::GraveSalt:
				stats.damage += 11.0f;
				break;
			case ReagentType::SaintAsh:
				stats.wardGain += 14.0f;
				break;
			case ReagentType::BlackWax:
				stats.poisonDamage += 4.0f;
				stats.hazardSeconds += 1.5f;
				break;
			case ReagentType::WidowNettle:
				stats.snareSeconds += 1.2f;
				break;
			case ReagentType::BellBronze:
				stats.splashRadius += 1.05f;
				stats.fearSeconds += 0.75f;
				break;
			case ReagentType::MothDust:
				stats.seeks = true;
				stats.damage += 4.0f;
				stats.fearSeconds += 0.45f;
				break;
			default:
				break;
		}
	}
	return stats;
}

std::string CurseVector::GetName() const {
	std::ostringstream result;
	switch (vector) {
		case VectorType::Bottle: result << "BOTTLE HEX"; break;
		case VectorType::Apple: result << "APPLE OMEN"; break;
		case VectorType::Spider: result << "SPIDER NEST"; break;
	}
	if (reagentCount == 0) result << " / POPPET FALLBACK";
	for (int i = 0; i < reagentCount; ++i) result << (i == 0 ? " + " : " / ") << ReagentName(reagents[i]);
	return result.str();
}
