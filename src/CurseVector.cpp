#include "CurseVector.hxx"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace {

constexpr std::size_t reagentIndex(ReagentType type) {
	return static_cast<std::size_t>(type);
}

void appendStat(std::ostringstream& stream, std::string_view label, float value, std::string_view suffix) {
	if (value <= 0.0f) {
		return;
	}

	stream << " | " << label << ' ' << std::fixed << std::setprecision(1) << value << suffix;
}

} // namespace

SpellStats& SpellStats::operator+=(const SpellStats& other) noexcept {
	damage += other.damage;
	radius += other.radius;
	snareDuration += other.snareDuration;
	poisonDuration += other.poisonDuration;
	fearDuration += other.fearDuration;
	homingStrength += other.homingStrength;
	lingeringDuration += other.lingeringDuration;
	wardGain += other.wardGain;
	return *this;
}

SpellStats operator+(SpellStats left, const SpellStats& right) noexcept {
	left += right;
	return left;
}

std::string_view curseVectorName(CurseVectorType type) noexcept {
	switch (type) {
	case CurseVectorType::Bottle:
		return "Bottle";
	case CurseVectorType::Apple:
		return "Apple";
	case CurseVectorType::Spider:
		return "Spider";
	case CurseVectorType::Poppet:
		return "Poppet";
	}

	return "Unknown";
}

std::string_view curseVectorDelivery(CurseVectorType type) noexcept {
	switch (type) {
	case CurseVectorType::Bottle:
		return "thrown curse";
	case CurseVectorType::Apple:
		return "eaten self-hex";
	case CurseVectorType::Spider:
		return "placed trap";
	case CurseVectorType::Poppet:
		return "weak needle curse";
	}

	return "unknown delivery";
}

SpellStats baseSpellStats(CurseVectorType type) noexcept {
	switch (type) {
	case CurseVectorType::Bottle:
		return {6.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	case CurseVectorType::Apple:
		return {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 9.0f};
	case CurseVectorType::Spider:
		return {6.0f, 1.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	case CurseVectorType::Poppet:
		return {5.0f, 0.20f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	}

	return {};
}

SelfHexStats selfHexStats(const SpellStats& spell) noexcept {
	SelfHexStats result {};
	result.healthRestore = spell.damage;
	result.wardGain = spell.wardGain + spell.radius * 9.0f;
	result.thornGuardDuration = spell.snareDuration;
	result.thornDamage = spell.snareDuration > 0.0f ? 6.0f + spell.damage * 0.15f : 0.0f;
	result.mendDuration = spell.poisonDuration;
	result.mendPerSecond = spell.poisonDuration > 0.0f ? 4.0f : 0.0f;
	result.hasteDuration = spell.fearDuration;
	result.speedMultiplier = spell.fearDuration > 0.0f
		? 1.15f + spell.homingStrength * 0.35f
		: 1.0f;
	result.waxShellDuration = spell.lingeringDuration;
	result.damageReduction = spell.lingeringDuration > 0.0f ? 0.35f : 0.0f;
	return result;
}

SpellStats reagentSpellStats(ReagentType type) noexcept {
	switch (type) {
	case ReagentType::GraveSalt:
		return {8.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	case ReagentType::ChurchGlass:
		return {0.0f, 0.65f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	case ReagentType::WidowNettle:
		return {0.0f, 0.0f, 1.25f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	case ReagentType::PlagueHoney:
		return {0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	case ReagentType::MothDust:
		return {0.0f, 0.0f, 0.0f, 0.0f, 1.5f, 0.35f, 0.0f, 0.0f};
	case ReagentType::BlackWax:
		return {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f};
	case ReagentType::Count:
		break;
	}

	return {};
}

CurseVector::CurseVector(CurseVectorType base) noexcept
	: base_(base) {
}

CurseVectorType CurseVector::base() const noexcept {
	return base_;
}

void CurseVector::setBase(CurseVectorType base) noexcept {
	base_ = base;
}

bool CurseVector::addReagent(ReagentType type) noexcept {
	if (!isValidReagentType(type) || reagentCount_ >= kMaxReagents) {
		return false;
	}

	reagents_[reagentCount_++] = type;
	return true;
}

bool CurseVector::removeReagent(std::size_t index) noexcept {
	if (index >= reagentCount_) {
		return false;
	}

	for (std::size_t current = index; current + 1 < reagentCount_; ++current) {
		reagents_[current] = reagents_[current + 1];
	}

	reagents_[--reagentCount_] = ReagentType::Count;
	return true;
}

bool CurseVector::removeLastReagent() noexcept {
	if (reagentCount_ == 0) {
		return false;
	}

	reagents_[--reagentCount_] = ReagentType::Count;
	return true;
}

void CurseVector::clearReagents() noexcept {
	reagents_.fill(ReagentType::Count);
	reagentCount_ = 0;
}

std::size_t CurseVector::reagentCount() const noexcept {
	return reagentCount_;
}

bool CurseVector::empty() const noexcept {
	return reagentCount_ == 0;
}

bool CurseVector::isPoppetFallback() const noexcept {
	return base_ == CurseVectorType::Poppet && empty();
}

std::optional<ReagentType> CurseVector::reagentAt(std::size_t index) const noexcept {
	if (index >= reagentCount_) {
		return std::nullopt;
	}

	return reagents_[index];
}

const std::array<ReagentType, CurseVector::kMaxReagents>& CurseVector::reagents() const noexcept {
	return reagents_;
}

SpellStats CurseVector::stats() const noexcept {
	SpellStats result = baseSpellStats(base_);
	for (std::size_t index = 0; index < reagentCount_; ++index) {
		result += reagentSpellStats(reagents_[index]);
	}

	if (base_ == CurseVectorType::Apple) {
		// The apple turns the assembled curse inward. Every selected reagent makes
		// the self-ward stronger without changing the reagent's normal payload.
		result.wardGain += static_cast<float>(reagentCount_) * 4.0f;
	}

	result.homingStrength = std::min(result.homingStrength, 1.0f);
	return result;
}

bool CurseVector::canConsume(const ReagentInventory& inventory) const noexcept {
	std::array<int, kReagentTypeCount> required {};
	for (std::size_t index = 0; index < reagentCount_; ++index) {
		++required[reagentIndex(reagents_[index])];
	}

	for (std::size_t index = 0; index < required.size(); ++index) {
		if (required[index] > 0 && inventory.count(static_cast<ReagentType>(index)) < required[index]) {
			return false;
		}
	}

	return true;
}

bool CurseVector::consume(ReagentInventory& inventory) const noexcept {
	if (!canConsume(inventory)) {
		return false;
	}

	for (std::size_t index = 0; index < reagentCount_; ++index) {
		inventory.remove(reagents_[index]);
	}

	return true;
}

std::string CurseVector::reagentSummary() const {
	if (empty()) {
		return "bare";
	}

	std::array<int, kReagentTypeCount> selected {};
	for (std::size_t index = 0; index < reagentCount_; ++index) {
		++selected[reagentIndex(reagents_[index])];
	}

	std::ostringstream stream;
	bool first = true;
	for (std::size_t index = 0; index < selected.size(); ++index) {
		if (selected[index] == 0) {
			continue;
		}

		if (!first) {
			stream << ", ";
		}

		stream << Reagent::definition(static_cast<ReagentType>(index)).name;
		if (selected[index] > 1) {
			stream << " x" << selected[index];
		}
		first = false;
	}

	return stream.str();
}

std::string CurseVector::preview() const {
	const SpellStats result = stats();
	std::ostringstream stream;
	stream << curseVectorName(base_) << " — " << curseVectorDelivery(base_);
	stream << " | " << reagentSummary();
	if (base_ == CurseVectorType::Apple) {
		const SelfHexStats self = selfHexStats(result);
		appendStat(stream, "VITALITY", self.healthRestore, "");
		appendStat(stream, "WARD", self.wardGain, "");
		appendStat(stream, "THORNS", self.thornGuardDuration, "s");
		appendStat(stream, "MEND", self.mendDuration, "s");
		appendStat(stream, "HASTE", self.hasteDuration, "s");
		appendStat(stream, "WAX", self.waxShellDuration, "s");
		return stream.str();
	}
	appendStat(stream, "DMG", result.damage, "");
	appendStat(stream, "R", result.radius, "m");
	appendStat(stream, "SNARE", result.snareDuration, "s");
	appendStat(stream, "ROT", result.poisonDuration, "s");
	appendStat(stream, "FEAR", result.fearDuration, "s");
	appendStat(stream, "SEEK", result.homingStrength * 100.0f, "%");
	appendStat(stream, "HAZARD", result.lingeringDuration, "s");
	appendStat(stream, "WARD", result.wardGain, "");
	return stream.str();
}

std::string CurseVector::preview(const ReagentInventory& inventory) const {
	std::string result = preview();
	result += canConsume(inventory) ? " | READY" : " | MISSING REAGENTS";
	return result;
}
