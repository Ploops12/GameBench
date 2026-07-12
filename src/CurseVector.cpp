#include "CurseVector.hxx"
#include <cstdio>

SpellStats CompileSpell(const SpellDraft& draft) {
	SpellStats stats;

	switch (draft.base) {
	case BaseVector::Poppet:
		stats.damage = 10.0f;
		stats.radius = 0.35f;
		stats.speed = 28.0f;
		break;
	case BaseVector::Bottle:
		stats.damage = 16.0f;
		stats.radius = 1.0f;
		stats.speed = 18.0f;
		break;
	case BaseVector::Apple:
		stats.damage = 0.0f;
		stats.radius = 2.0f;
		stats.speed = 0.0f;
		stats.wardGain = 12.0f;
		stats.duration = 4.0f;
		break;
	case BaseVector::Spider:
		stats.damage = 14.0f;
		stats.radius = 1.3f;
		stats.speed = 0.0f;
		stats.duration = 12.0f;
		break;
	case BaseVector::Count:
		break;
	}

	for (int i = 0; i < draft.reagentCount; ++i) {
		switch (draft.reagents[i]) {
		case ReagentType::GraveSalt:
			stats.damage += 14.0f;
			stats.radius += 0.15f;
			break;
		case ReagentType::WidowNettle:
			stats.damage += 5.0f;
			stats.poisonDps += 5.0f;
			stats.snareSeconds += 1.4f;
			break;
		case ReagentType::MothDust:
			stats.fearSeconds += 2.0f;
			stats.radius += 0.25f;
			break;
		case ReagentType::BlackWax:
			stats.lingering = true;
			stats.duration += 3.0f;
			stats.radius += 0.35f;
			break;
		case ReagentType::SaintAsh:
			stats.wardGain += 18.0f;
			stats.damage += 2.0f;
			break;
		case ReagentType::BellFilings:
			stats.seekStrength += 3.0f;
			stats.speed += 2.0f;
			stats.damage += 4.0f;
			break;
		case ReagentType::Count:
			break;
		}
	}

	if (draft.base == BaseVector::Apple) {
		stats.damage *= 0.45f;
		stats.wardGain += 8.0f * draft.reagentCount;
	}

	return stats;
}

std::string BuildSpellPreview(const SpellDraft& draft) {
	SpellStats stats = CompileSpell(draft);
	char buffer[256];
	std::snprintf(
		buffer,
		sizeof(buffer),
		"%s | dmg %.0f radius %.1f ward +%.0f%s%s%s",
		GetBaseVectorDefinition(draft.base).name.data(),
		stats.damage,
		stats.radius,
		stats.wardGain,
		stats.snareSeconds > 0.0f ? " snare" : "",
		stats.fearSeconds > 0.0f ? " fear" : "",
		stats.lingering ? " lingering" : "");

	return std::string(buffer);
}
