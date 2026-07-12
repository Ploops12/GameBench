#include <algorithm>
#include <sstream>
#include "CurseVector.hxx"

SpellRecipe::SpellRecipe() {
	counts.fill(0);
}

int SpellRecipe::totalSelections() const {
	return slotCount;
}

int SpellRecipe::getCount(ReagentType type) const {
	return counts[getReagentIndex(type)];
}

bool SpellRecipe::addReagent(ReagentType type, int availableCount) {
	if (slotCount >= static_cast<int>(slots.size())) {
		return false;
	}

	const int index = getReagentIndex(type);
	if (counts[index] >= availableCount) {
		return false;
	}

	slots[slotCount] = type;
	++slotCount;
	++counts[index];
	return true;
}

void SpellRecipe::popSelection() {
	if (slotCount <= 0) {
		return;
	}

	--slotCount;
	ReagentType removed = slots[slotCount];
	--counts[getReagentIndex(removed)];
}

void SpellRecipe::clearReagents() {
	slotCount = 0;
	counts.fill(0);
}

const char* getBaseVectorName(BaseVectorType base) {
	switch (base) {
		case BaseVectorType::Poppet: return "Poppet";
		case BaseVectorType::Bottle: return "Bottle";
		case BaseVectorType::Apple: return "Apple";
		case BaseVectorType::Spider: return "Spider";
	}
	return "Unknown";
}

Color getBaseVectorColor(BaseVectorType base) {
	switch (base) {
		case BaseVectorType::Poppet: return {210, 180, 165, 255};
		case BaseVectorType::Bottle: return {255, 136, 92, 255};
		case BaseVectorType::Apple: return {176, 255, 152, 255};
		case BaseVectorType::Spider: return {197, 161, 255, 255};
	}
	return WHITE;
}

static std::string buildSpellName(const SpellRecipe& recipe) {
	if (recipe.totalSelections() == 0) {
		return "Poppet Hex";
	}

	int strongestCount = 0;
	ReagentType strongest = ReagentType::GraveSalt;
	for (const ReagentInfo& info : getReagentInfos()) {
		const int count = recipe.counts[getReagentIndex(info.type)];
		if (count > strongestCount) {
			strongestCount = count;
			strongest = info.type;
		}
	}

	std::ostringstream label;
	label << getReagentInfo(strongest).name << " " << getBaseVectorName(recipe.base);
	return label.str();
}

ComposedSpell composeSpell(const SpellRecipe& recipe) {
	ComposedSpell spell;
	spell.base = recipe.totalSelections() == 0 ? BaseVectorType::Poppet : recipe.base;
	spell.counts = recipe.counts;
	spell.label = buildSpellName(recipe);
	spell.fallbackPoppet = recipe.totalSelections() == 0;

	switch (spell.base) {
		case BaseVectorType::Poppet:
			spell.damage = 10.0f;
			spell.projectileSpeed = 18.0f;
			spell.splashRadius = 0.45f;
			spell.lifetime = 1.0f;
			break;
		case BaseVectorType::Bottle:
			spell.damage = 14.0f;
			spell.projectileSpeed = 12.0f;
			spell.splashRadius = 1.0f;
			spell.lifetime = 4.0f;
			spell.hazardDuration = 1.0f;
			break;
		case BaseVectorType::Apple:
			spell.damage = 6.0f;
			spell.projectileSpeed = 0.0f;
			spell.splashRadius = 2.2f;
			spell.lifetime = 0.1f;
			spell.wardGain = 8.0f;
			break;
		case BaseVectorType::Spider:
			spell.damage = 8.0f;
			spell.projectileSpeed = 0.0f;
			spell.splashRadius = 1.6f;
			spell.lifetime = 12.0f;
			spell.triggerRadius = 1.4f;
			spell.hazardDuration = 1.5f;
			break;
	}

	const int salt = recipe.getCount(ReagentType::GraveSalt);
	const int ash = recipe.getCount(ReagentType::SaintAsh);
	const int nettle = recipe.getCount(ReagentType::WidowNettle);
	const int oil = recipe.getCount(ReagentType::FuneralOil);
	const int moth = recipe.getCount(ReagentType::MothDust);
	const int glass = recipe.getCount(ReagentType::ChurchGlass);

	spell.damage += salt * 7.0f;
	spell.wardGain += ash * 9.0f;
	spell.snareDuration += nettle * 0.85f;
	spell.rotDamagePerSecond += oil * 3.2f;
	spell.rotDuration += oil * 2.2f;
	spell.hazardDuration += oil * 1.2f + nettle * 0.6f;
	spell.fearDuration += moth * 0.9f;
	spell.homingStrength += moth * 0.22f;
	spell.splashRadius += glass * 0.38f;
	spell.damage += glass * 2.0f;

	if (spell.base == BaseVectorType::Bottle) {
		spell.projectileSpeed += glass * 0.8f;
	}
	if (spell.base == BaseVectorType::Spider) {
		spell.triggerRadius += glass * 0.2f;
		spell.lifetime += 2.0f + nettle * 0.5f;
	}
	if (spell.base == BaseVectorType::Apple) {
		spell.wardGain += 6.0f;
		spell.hazardDuration *= 0.5f;
	}

	std::ostringstream preview;
	preview << getBaseVectorName(spell.base) << "  dmg " << static_cast<int>(spell.damage);
	if (spell.splashRadius > 0.6f) {
		preview << "  rad " << spell.splashRadius;
	}
	if (spell.snareDuration > 0.0f) {
		preview << "  snare " << spell.snareDuration << "s";
	}
	if (spell.rotDuration > 0.0f) {
		preview << "  rot " << spell.rotDamagePerSecond << "/s";
	}
	if (spell.fearDuration > 0.0f) {
		preview << "  fear " << spell.fearDuration << "s";
	}
	if (spell.wardGain > 0.0f) {
		preview << "  ward +" << static_cast<int>(spell.wardGain);
	}
	spell.preview = preview.str();
	return spell;
}
