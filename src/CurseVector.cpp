#include <algorithm>
#include <cmath>
#include <cstdio>
#include "CurseVector.hxx"

namespace {
	constexpr int kMaxSelections = 3;

	float baseDamage(BaseVectorType base) {
		switch (base) {
			case BaseVectorType::Bottle:
				return 12.0f;
			case BaseVectorType::Apple:
				return 8.0f;
			case BaseVectorType::Spider:
				return 11.0f;
			case BaseVectorType::Poppet:
			default:
				return 9.0f;
		}
	}

	float baseRadius(BaseVectorType base) {
		switch (base) {
			case BaseVectorType::Bottle:
				return 1.7f;
			case BaseVectorType::Apple:
				return 3.1f;
			case BaseVectorType::Spider:
				return 2.1f;
			case BaseVectorType::Poppet:
			default:
				return 0.55f;
		}
	}

	float baseCooldown(BaseVectorType base) {
		switch (base) {
			case BaseVectorType::Bottle:
				return 0.55f;
			case BaseVectorType::Apple:
				return 0.9f;
			case BaseVectorType::Spider:
				return 0.8f;
			case BaseVectorType::Poppet:
			default:
				return 0.24f;
		}
	}
}

CurseVector::CurseVector() = default;

void CurseVector::setBase(BaseVectorType value) {
	base = value;
}

BaseVectorType CurseVector::getBase() const {
	return base;
}

bool CurseVector::addReagent(ReagentType type, const std::array<int, ReagentTypeCount>& inventory) {
	const int index = static_cast<int>(type);
	if (inventory[index] <= 0) {
		return false;
	}

	auto stacks = getStacks();
	if (stacks[index] >= inventory[index]) {
		return false;
	}

	if (static_cast<int>(selections.size()) >= kMaxSelections) {
		return false;
	}

	selections.push_back(type);
	return true;
}

bool CurseVector::removeLast() {
	if (selections.empty()) {
		return false;
	}

	selections.pop_back();
	return true;
}

void CurseVector::clear() {
	selections.clear();
}

void CurseVector::pruneToInventory(const std::array<int, ReagentTypeCount>& inventory) {
	auto stacks = getStacks();
	bool changed = true;
	while (changed) {
		changed = false;
		for (int i = 0; i < ReagentTypeCount; ++i) {
			if (stacks[i] > inventory[i]) {
				auto it = std::find(selections.rbegin(), selections.rend(), static_cast<ReagentType>(i));
				if (it != selections.rend()) {
					selections.erase(std::next(it).base());
					stacks[i] -= 1;
					changed = true;
				}
			}
		}
	}
}

int CurseVector::selectionCount() const {
	return static_cast<int>(selections.size());
}

const std::vector<ReagentType>& CurseVector::getSelections() const {
	return selections;
}

std::array<int, ReagentTypeCount> CurseVector::getStacks() const {
	std::array<int, ReagentTypeCount> stacks {};
	for (ReagentType type : selections) {
		stacks[static_cast<int>(type)] += 1;
	}
	return stacks;
}

SpellPreview CurseVector::buildPreview(bool allowFallbackPoppet) const {
	SpellPreview preview;
	preview.base = base;
	preview.totalSelections = static_cast<int>(selections.size());
	preview.stacks = getStacks();
	preview.fallbackPoppet = allowFallbackPoppet && preview.base == BaseVectorType::Poppet;

	preview.damage = baseDamage(preview.base);
	preview.radius = baseRadius(preview.base);
	preview.cooldown = baseCooldown(preview.base);
	preview.projectileSpeed = preview.base == BaseVectorType::Poppet ? 16.0f : 12.5f;
	preview.lifetime = preview.base == BaseVectorType::Spider ? 12.0f : 3.0f;
	preview.lingerDuration = preview.base == BaseVectorType::Spider ? 4.5f : 0.0f;

	preview.damage += preview.stacks[static_cast<int>(ReagentType::GraveSalt)] * 9.0f;
	preview.wardGain += preview.stacks[static_cast<int>(ReagentType::SaintAsh)] * 7.0f;
	preview.snareDuration += preview.stacks[static_cast<int>(ReagentType::WidowNettle)] * 0.95f;
	preview.seekStrength += preview.stacks[static_cast<int>(ReagentType::BellBronze)] * 0.35f;
	preview.fearDuration += preview.stacks[static_cast<int>(ReagentType::MothDust)] * 1.2f;
	preview.rotDamagePerSecond += preview.stacks[static_cast<int>(ReagentType::BogRot)] * 4.0f;
	preview.rotDuration += preview.stacks[static_cast<int>(ReagentType::BogRot)] * 2.0f;
	preview.radius += preview.stacks[static_cast<int>(ReagentType::BellBronze)] * 0.35f;

	if (preview.base == BaseVectorType::Apple) {
		preview.radius += 0.3f;
		preview.wardGain += 4.0f;
	}

	if (preview.base == BaseVectorType::Spider) {
		preview.lingerDuration += preview.rotDuration > 0.0f || preview.snareDuration > 0.0f ? 2.5f : 0.0f;
		preview.lifetime = 14.0f;
	}

	return preview;
}

const char* GetBaseVectorName(BaseVectorType base) {
	switch (base) {
		case BaseVectorType::Bottle:
			return "Bottle";
		case BaseVectorType::Apple:
			return "Apple";
		case BaseVectorType::Spider:
			return "Spider";
		case BaseVectorType::Poppet:
		default:
			return "Poppet";
	}
}

const char* GetBaseVectorShortName(BaseVectorType base) {
	switch (base) {
		case BaseVectorType::Bottle:
			return "Thrown curse";
		case BaseVectorType::Apple:
			return "Self pulse";
		case BaseVectorType::Spider:
			return "Placed mine";
		case BaseVectorType::Poppet:
		default:
			return "Weak dart";
	}
}

std::vector<std::string> DescribeSpell(const SpellPreview& preview) {
	std::vector<std::string> lines;

	char line[128];
	std::snprintf(line, sizeof(line), "%s - %s", GetBaseVectorName(preview.base), GetBaseVectorShortName(preview.base));
	lines.emplace_back(line);

	std::snprintf(line, sizeof(line), "Damage %.0f  Radius %.1f", preview.damage, preview.radius);
	lines.emplace_back(line);

	if (preview.rotDamagePerSecond > 0.0f) {
		std::snprintf(line, sizeof(line), "Rot %.0f/s for %.0fs", preview.rotDamagePerSecond, preview.rotDuration);
		lines.emplace_back(line);
	}

	if (preview.snareDuration > 0.0f) {
		std::snprintf(line, sizeof(line), "Snare %.1fs", preview.snareDuration);
		lines.emplace_back(line);
	}

	if (preview.fearDuration > 0.0f) {
		std::snprintf(line, sizeof(line), "Panic %.1fs", preview.fearDuration);
		lines.emplace_back(line);
	}

	if (preview.seekStrength > 0.0f) {
		std::snprintf(line, sizeof(line), "Seeking %.0f%%", preview.seekStrength * 100.0f);
		lines.emplace_back(line);
	}

	if (preview.wardGain > 0.0f) {
		std::snprintf(line, sizeof(line), "Ward gain %.0f", preview.wardGain);
		lines.emplace_back(line);
	}

	if (preview.base == BaseVectorType::Spider && preview.lingerDuration > 0.0f) {
		std::snprintf(line, sizeof(line), "Lingering blight %.1fs", preview.lingerDuration);
		lines.emplace_back(line);
	}

	if (preview.fallbackPoppet) {
		lines.emplace_back("No ingredients: casts the weak poppet dart.");
	}

	return lines;
}
