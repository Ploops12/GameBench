#include "CurseVector.hxx"

const char* reagentName(ReagentType type) {
	switch (type) {
	case ReagentType::GraveSalt: return "Grave Salt";
	case ReagentType::WidowNettle: return "Widow Nettle";
	case ReagentType::MothDust: return "Moth Dust";
	case ReagentType::PlagueHoney: return "Plague Honey";
	case ReagentType::SaintAsh: return "Saint Ash";
	case ReagentType::BellBronze: return "Bell Bronze";
	default: return "Unknown";
	}
}

const char* reagentShortEffect(ReagentType type) {
	switch (type) {
	case ReagentType::GraveSalt: return "damage";
	case ReagentType::WidowNettle: return "snare";
	case ReagentType::MothDust: return "fear";
	case ReagentType::PlagueHoney: return "rot";
	case ReagentType::SaintAsh: return "ward";
	case ReagentType::BellBronze: return "seeking";
	default: return "";
	}
}

void CurseVector::rebuild() {
	damage = base == CurseBase::Poppet ? 12.0f : 20.0f;
	radius = base == CurseBase::Bottle ? 1.6f : base == CurseBase::Apple ? 2.8f : 0.0f;
	duration = base == CurseBase::Spider ? 12.0f : 0.0f;
	seeking = snare = poison = fear = wardGain = false;
	for (int i = 0; i < reagentCount; ++i) {
		switch (reagents[i]) {
		case ReagentType::GraveSalt: damage += 12.0f; break;
		case ReagentType::WidowNettle: snare = true; duration += 1.5f; break;
		case ReagentType::MothDust: fear = true; duration += 1.5f; break;
		case ReagentType::PlagueHoney: poison = true; duration += 2.5f; break;
		case ReagentType::SaintAsh: wardGain = true; break;
		case ReagentType::BellBronze: seeking = true; break;
		default: break;
		}
	}
}

const char* CurseVector::baseName() const { switch (base) { case CurseBase::Poppet: return "Poppet"; case CurseBase::Bottle: return "Bottle"; case CurseBase::Apple: return "Apple"; case CurseBase::Spider: return "Spider"; } return ""; }
const char* CurseVector::deliveryName() const { switch (base) { case CurseBase::Poppet: return "needle bolt"; case CurseBase::Bottle: return "thrown curse"; case CurseBase::Apple: return "eaten omen"; case CurseBase::Spider: return "placed snare"; } return ""; }
