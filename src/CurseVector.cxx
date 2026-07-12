#include "CurseVector.hxx"

#include <sstream>

bool CurseVector::addReagent(ReagentType type) {
	const int value = static_cast<int>(type);
	for (int &slot : reagents) {
		if (slot == -1) { slot = value; return true; }
	}
	return false;
}

void CurseVector::removeLastReagent() {
	for (int index = static_cast<int>(reagents.size()) - 1; index >= 0; --index) {
		if (reagents[index] != -1) { reagents[index] = -1; return; }
	}
}

SpellStats CurseVector::stats() const {
	SpellStats result;
	if (base == BaseVector::Poppet) result = {8, 0.0f, 0.0f, 0.0f, 0.0f, 0, false, false, "poppet sting"};
	std::string effects;
	for (int slot : reagents) {
		if (slot < 0) continue;
		switch (static_cast<ReagentType>(slot)) {
			case ReagentType::GraveSalt: result.damage += 7; result.radius += 0.55f; effects += "salt splash, "; break;
			case ReagentType::SaintAsh: result.wardGain += 10; effects += "ward leech, "; break;
			case ReagentType::BlackWax: result.seeking += 1.0f; effects += "seeking, "; break;
			case ReagentType::WidowNettle: result.snare += 1.8f; effects += "thorn snare, "; break;
			case ReagentType::MothDust: result.duration += 2.0f; result.panic = true; effects += "panic haze, "; break;
			case ReagentType::BogMarrow: result.damage += 4; result.duration += 1.5f; result.rot = true; effects += "rot, "; break;
			default: break;
		}
	}
	if (!effects.empty()) { effects.pop_back(); effects.pop_back(); }
	if (!effects.empty()) result.effects = effects;
	if (base == BaseVector::Spider) result.radius += 1.0f;
	if (base == BaseVector::Apple) result.wardGain += 8;
	return result;
}

std::string CurseVector::preview() const {
	const SpellStats result = stats();
	const char *baseName = base == BaseVector::Bottle ? "BOTTLE" : base == BaseVector::Apple ? "APPLE" : base == BaseVector::Spider ? "SPIDER" : "POPPET";
	std::ostringstream text;
	text << baseName << "  |  DMG " << result.damage;
	if (result.radius > 0.0f) text << "  SPLASH " << static_cast<int>(result.radius * 10.0f);
	if (result.snare > 0.0f) text << "  SNARE";
	if (result.seeking > 0.0f) text << "  SEEK";
	if (result.wardGain > 0) text << "  WARD +" << result.wardGain;
	text << "  " << result.effects;
	return text.str();
}
