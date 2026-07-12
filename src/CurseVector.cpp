#include "CurseVector.hxx"

#include <algorithm>
#include <array>
#include <sstream>

CurseVector::CurseVector() = default;

void CurseVector::selectBase(BaseVectorType base) {
	base_ = base;
	if (base_ == BaseVectorType::Poppet) {
		reagents_.clear();
	}
}

BaseVectorType CurseVector::base() const {
	return base_;
}

bool CurseVector::addReagent(ReagentType type) {
	if (base_ == BaseVectorType::Poppet || type == ReagentType::Count || reagents_.size() >= MAX_REAGENTS) {
		return false;
	}
	reagents_.push_back(type);
	return true;
}

bool CurseVector::removeLastReagent() {
	if (reagents_.empty()) {
		return false;
	}
	reagents_.pop_back();
	return true;
}

void CurseVector::clearReagents() {
	reagents_.clear();
}

const std::vector<ReagentType>& CurseVector::reagents() const {
	return reagents_;
}

int CurseVector::reagentCount(ReagentType type) const {
	return static_cast<int>(std::count(reagents_.begin(), reagents_.end(), type));
}

bool CurseVector::isFallback() const {
	return base_ == BaseVectorType::Poppet;
}

SpellStats CurseVector::stats() const {
	SpellStats result;
	switch (base_) {
	case BaseVectorType::Bottle:
		result.damage = 16.0f;
		result.radius = 1.0f;
		result.duration = 2.0f;
		result.projectileSpeed = 17.0f;
		result.cooldown = 0.48f;
		break;
	case BaseVectorType::Apple:
		result.radius = 2.4f;
		result.duration = 4.0f;
		result.heal = 12.0f;
		result.cooldown = 1.6f;
		break;
	case BaseVectorType::Spider:
		result.damage = 22.0f;
		result.radius = 1.5f;
		result.duration = 3.0f;
		result.cooldown = 0.65f;
		break;
	case BaseVectorType::Poppet:
		result.damage = 12.0f;
		result.cooldown = 0.32f;
		break;
	}

	for (const ReagentType reagent : reagents_) {
		switch (reagent) {
		case ReagentType::BoneSplinters:
			result.damage += 14.0f;
			break;
		case ReagentType::GraveSalt:
			result.radius += 0.9f;
			break;
		case ReagentType::WidowNettle:
			result.snareStrength += 0.22f;
			result.duration += 1.0f;
			break;
		case ReagentType::Rotcap:
			result.poisonDps += 5.0f;
			result.duration += 0.8f;
			result.lingering = true;
			break;
		case ReagentType::MothDust:
			result.homingStrength += 4.0f;
			result.fearDuration += 0.9f;
			break;
		case ReagentType::SaintAsh:
			result.wardGain += 18.0f;
			break;
		case ReagentType::Count:
			break;
		}
	}
	result.snareStrength = std::min(result.snareStrength, 0.75f);
	return result;
}

std::string CurseVector::name() const {
	if (base_ == BaseVectorType::Poppet) {
		return "Threadbare Poppet";
	}
	if (reagents_.empty()) {
		switch (base_) {
		case BaseVectorType::Bottle: return "Empty Bottle Hex";
		case BaseVectorType::Apple: return "Bitter Apple";
		case BaseVectorType::Spider: return "Watch-Spider";
		case BaseVectorType::Poppet: break;
		}
	}

	const ReagentType first = reagents_.front();
	const int copies = reagentCount(first);
	std::string prefix;
	if (copies == static_cast<int>(reagents_.size()) && copies > 1) {
		prefix = copies == 2 ? "Double " : "Triple ";
	}
	prefix += std::string(GetReagentDefinition(first).shortName);
	return prefix + " " + baseName() + " Hex";
}

std::string CurseVector::baseName() const {
	switch (base_) {
	case BaseVectorType::Bottle: return "Bottle";
	case BaseVectorType::Apple: return "Apple";
	case BaseVectorType::Spider: return "Spider";
	case BaseVectorType::Poppet: return "Poppet";
	}
	return "Unknown";
}

std::string CurseVector::ingredientSummary() const {
	if (reagents_.empty()) {
		return "none";
	}
	std::ostringstream result;
	for (std::size_t i = 0; i < reagents_.size(); ++i) {
		if (i > 0) {
			result << " + ";
		}
		result << GetReagentDefinition(reagents_[i]).shortName;
	}
	return result.str();
}

bool CurseVector::canConsume(const ReagentInventory& inventory) const {
	std::array<int, REAGENT_TYPE_COUNT> needed {};
	for (const ReagentType reagent : reagents_) {
		++needed[ReagentIndex(reagent)];
	}
	for (std::size_t i = 0; i < needed.size(); ++i) {
		if (needed[i] > inventory[i]) {
			return false;
		}
	}
	return true;
}
