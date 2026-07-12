#include "Reagent.hxx"

const char* reagentName(ReagentType type) {
	switch (type) {
	case ReagentType::GraveSalt: return "GRAVE SALT";
	case ReagentType::WidowNettle: return "WIDOW NETTLE";
	case ReagentType::BogMarrow: return "BOG MARROW";
	case ReagentType::MothDust: return "MOTH DUST";
	case ReagentType::SaintAsh: return "SAINT ASH";
	case ReagentType::ThornHeart: return "THORN HEART";
	default: return "UNKNOWN";
	}
}

const char* reagentShortEffect(ReagentType type) {
	switch (type) {
	case ReagentType::GraveSalt: return "+12 harm";
	case ReagentType::WidowNettle: return "+1.25s snare";
	case ReagentType::BogMarrow: return "+rot / duration";
	case ReagentType::MothDust: return "+seek / panic";
	case ReagentType::SaintAsh: return "+15 ward";
	case ReagentType::ThornHeart: return "+splash / briars";
	default: return "";
	}
}

Color reagentColor(ReagentType type) {
	switch (type) {
	case ReagentType::GraveSalt: return Color{220, 220, 205, 255};
	case ReagentType::WidowNettle: return Color{105, 210, 110, 255};
	case ReagentType::BogMarrow: return Color{170, 120, 75, 255};
	case ReagentType::MothDust: return Color{205, 175, 95, 255};
	case ReagentType::SaintAsh: return Color{110, 215, 225, 255};
	case ReagentType::ThornHeart: return Color{220, 70, 90, 255};
	default: return WHITE;
	}
}
