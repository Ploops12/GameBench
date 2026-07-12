#include "CurseVector.hxx"

#include <cstdio>

const ReagentInfo& GetReagentInfo(ReagentType type) {
	static const std::array<ReagentInfo, ReagentCount> info {{
		{"GRAVE SALT", "damage + splash", {220, 218, 191, 255}},
		{"WIDOW NETTLE", "thorn snare", {114, 205, 92, 255}},
		{"PLAGUE HONEY", "rot over time", {211, 157, 47, 255}},
		{"MOTH DUST", "panic + seeking", {127, 190, 213, 255}},
		{"SAINT ASH", "ward on hit", {190, 190, 205, 255}},
		{"RAVEN EYE", "lingering curse", {213, 72, 70, 255}}
	}};
	return info[static_cast<int>(type)];
}

SpellStats CurseVector::stats() const {
	SpellStats out;
	if (type == VectorType::Bottle) {
		out.damage = 18.0f;
		out.radius = 1.4f;
	} else if (type == VectorType::Apple) {
		out.damage = 0.0f;
		out.wardGain = 12.0f;
	} else if (type == VectorType::Spider) {
		out.damage = 15.0f;
		out.radius = 1.8f;
		out.duration = 7.0f;
	}
	for (int i = 0; i < reagentSlots; ++i) {
		switch (reagents[i]) {
		case ReagentType::GraveSalt: out.damage += 10.0f; out.radius += 0.45f; break;
		case ReagentType::WidowNettle: out.snareSeconds += 1.6f; break;
		case ReagentType::PlagueHoney: out.poisonDamage += 7.0f; out.duration += 1.5f; break;
		case ReagentType::MothDust: out.fearSeconds += 1.2f; out.homing += 2.6f; break;
		case ReagentType::SaintAsh: out.wardGain += 14.0f; break;
		case ReagentType::RavenEye: out.duration += 2.5f; out.radius += 0.2f; break;
		default: break;
		}
	}
	return out;
}

const char* CurseVector::name() const {
	switch (type) {
	case VectorType::Bottle: return "BOTTLE HEX";
	case VectorType::Apple: return "BITTER APPLE";
	case VectorType::Spider: return "SPIDER SNARE";
	default: return "POPPET PRICK";
	}
}

std::string CurseVector::ingredients() const {
	if (reagentSlots == 0) return "NO REAGENTS // POPPET REMAINS AVAILABLE";
	std::string out;
	for (int i = 0; i < reagentSlots; ++i) {
		if (i > 0) out += " + ";
		out += GetReagentInfo(reagents[i]).name;
	}
	return out;
}

std::string CurseVector::preview() const {
	const SpellStats s = stats();
	char text[192];
	std::snprintf(text, sizeof(text), "DMG %.0f  SPLASH %.1fm  ROT %.0f  SNARE %.1fs  PANIC %.1fs  WARD +%.0f  LINGER %.1fs",
		s.damage, s.radius, s.poisonDamage, s.snareSeconds, s.fearSeconds, s.wardGain, s.duration);
	return text;
}
