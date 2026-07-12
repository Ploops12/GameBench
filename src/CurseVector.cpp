#include "CurseVector.hxx"
#include <iomanip>
#include <sstream>

CurseVector::CurseVector(BaseVector base) : base(base) {}

bool CurseVector::addReagent(ReagentType reagent) {
	if (reagentCount >= static_cast<int>(reagents.size())) return false;
	reagents[reagentCount++] = reagent;
	return true;
}

void CurseVector::clearReagents() {
	reagentCount = 0;
}

SpellStats CurseVector::compose() const {
	SpellStats stats;
	switch (base) {
	case BaseVector::Bottle:
		stats.damage = 16.0f;
		stats.splashRadius = 0.7f;
		break;
	case BaseVector::Apple:
		stats.damage = 5.0f;
		stats.splashRadius = 3.0f;
		stats.healAmount = 18.0f;
		stats.wardGain = 8.0f;
		break;
	case BaseVector::Spider:
		stats.damage = 20.0f;
		stats.splashRadius = 2.0f;
		stats.lingeringDuration = 1.0f;
		break;
	case BaseVector::Poppet:
		stats.damage = 8.0f;
		break;
	}

	for (int i = 0; i < reagentCount; ++i) {
		switch (reagents[i]) {
		case ReagentType::GraveSalt: stats.damage += 12.0f; break;
		case ReagentType::WidowNettle: stats.snareDuration += 1.25f; break;
		case ReagentType::BogMarrow:
			stats.rotDamagePerSecond += 4.0f;
			stats.lingeringDuration += 2.0f;
			break;
		case ReagentType::MothDust:
			stats.homingStrength++;
			stats.fearDuration += 1.0f;
			break;
		case ReagentType::SaintAsh: stats.wardGain += 15.0f; break;
		case ReagentType::ThornHeart:
			stats.splashRadius += 1.2f;
			stats.lingeringDuration += 1.5f;
			break;
		default: break;
		}
	}
	return stats;
}

std::string CurseVector::preview() const {
	const SpellStats stats = compose();
	std::ostringstream out;
	out << baseName(base) << "  HARM " << static_cast<int>(stats.damage);
	if (stats.splashRadius > 0.0f) out << "  RAD " << std::fixed << std::setprecision(1) << stats.splashRadius;
	if (stats.snareDuration > 0.0f) out << "  SNARE " << std::setprecision(1) << stats.snareDuration << "s";
	if (stats.rotDamagePerSecond > 0.0f) out << "  ROT " << static_cast<int>(stats.rotDamagePerSecond) << "/s";
	if (stats.fearDuration > 0.0f) out << "  PANIC " << std::setprecision(1) << stats.fearDuration << "s";
	if (stats.wardGain > 0.0f) out << "  WARD +" << static_cast<int>(stats.wardGain);
	if (stats.lingeringDuration > 1.0f) out << "  LINGER " << std::setprecision(1) << stats.lingeringDuration << "s";
	if (stats.homingStrength > 0) out << "  SEEK " << stats.homingStrength;
	return out.str();
}

const char* CurseVector::baseName(BaseVector value) {
	switch (value) {
	case BaseVector::Bottle: return "BOTTLE HEX";
	case BaseVector::Apple: return "BITTER APPLE";
	case BaseVector::Spider: return "WIDOW MINE";
	case BaseVector::Poppet: return "POPPET JAB";
	default: return "CURSE";
	}
}
