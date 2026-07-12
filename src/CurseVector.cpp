#include "CurseVector.hxx"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace {
bool isValidReagent(ReagentType reagent) {
	return static_cast<std::size_t>(reagent) < REAGENT_TYPE_COUNT;
}

std::string oneDecimal(float value) {
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(1) << value;
	return stream.str();
}

void appendPreviewPart(std::ostringstream& stream, const std::string& part) {
	if (stream.tellp() > 0) stream << " | ";
	stream << part;
}
}

CurseVector::CurseVector(CurseVectorType type) : type(type) {
}

CurseVectorType CurseVector::getType() const {
	return type;
}

void CurseVector::setType(CurseVectorType value) {
	type = value;
	if (type == CurseVectorType::Poppet) clearReagents();
}

bool CurseVector::addReagent(ReagentType reagent, const ReagentInventory& inventory) {
	if (type == CurseVectorType::Poppet || !isValidReagent(reagent) || reagentCount >= MAX_REAGENTS) {
		return false;
	}

	const std::size_t index = static_cast<std::size_t>(reagent);
	if (inventory[index] <= getDoseCount(reagent)) return false;

	reagents[reagentCount++] = reagent;
	return true;
}

bool CurseVector::removeLastReagent() {
	if (reagentCount <= 0) return false;
	--reagentCount;
	return true;
}

bool CurseVector::removeReagent(ReagentType reagent) {
	for (int i = reagentCount - 1; i >= 0; --i) {
		if (reagents[i] != reagent) continue;
		for (int j = i; j < reagentCount - 1; ++j) reagents[j] = reagents[j + 1];
		--reagentCount;
		return true;
	}
	return false;
}

void CurseVector::clearReagents() {
	reagentCount = 0;
}

int CurseVector::getReagentCount() const {
	return reagentCount;
}

ReagentType CurseVector::getReagent(int index) const {
	if (index < 0 || index >= reagentCount) return ReagentType::Count;
	return reagents[index];
}

int CurseVector::getDoseCount(ReagentType reagent) const {
	if (!isValidReagent(reagent)) return 0;
	return static_cast<int>(std::count(reagents.begin(), reagents.begin() + reagentCount, reagent));
}

bool CurseVector::canAfford(const ReagentInventory& inventory) const {
	std::array<int, REAGENT_TYPE_COUNT> needed {};
	for (int i = 0; i < reagentCount; ++i) {
		if (!isValidReagent(reagents[i])) return false;
		++needed[static_cast<std::size_t>(reagents[i])];
	}
	for (std::size_t i = 0; i < REAGENT_TYPE_COUNT; ++i) {
		if (needed[i] > inventory[i]) return false;
	}
	return true;
}

bool CurseVector::consume(ReagentInventory& inventory) const {
	if (!canAfford(inventory)) return false;
	for (int i = 0; i < reagentCount; ++i) {
		--inventory[static_cast<std::size_t>(reagents[i])];
	}
	return true;
}

SpellStats CurseVector::calculateStats() const {
	SpellStats stats;
	switch (type) {
		case CurseVectorType::Bottle:
			stats.impactDamage = 7.0f;
			stats.radius = 0.75f;
			stats.cooldown = 0.55f;
			stats.projectileSpeed = 20.0f;
			break;
		case CurseVectorType::Apple:
			stats.radius = 2.25f;
			stats.selfHeal = 6.0f;
			stats.hasteDuration = 1.25f;
			stats.cooldown = 0.85f;
			break;
		case CurseVectorType::Spider:
			stats.impactDamage = 5.0f;
			stats.radius = 2.0f;
			stats.cooldown = 0.8f;
			break;
		case CurseVectorType::Poppet:
			stats.impactDamage = 9.0f;
			stats.radius = 0.15f;
			stats.cooldown = 0.3f;
			stats.projectileSpeed = 38.0f;
			return stats;
	}

	for (int i = 0; i < reagentCount; ++i) {
		switch (reagents[i]) {
			case ReagentType::BoneShards:
				stats.impactDamage += 16.0f;
				break;
			case ReagentType::WidowNettle:
				stats.snareDuration += 1.5f;
				break;
			case ReagentType::Rotcap:
				stats.rotDamagePerSecond += 3.5f;
				stats.rotDuration += 2.75f;
				stats.lingeringDuration += 2.0f;
				break;
			case ReagentType::BatWing:
				stats.fearDuration += 1.4f;
				break;
			case ReagentType::WitchGlass:
				stats.wardGain += 18.0f;
				break;
			case ReagentType::PreservedEye:
				stats.seekingStrength += 0.34f;
				stats.radius += 0.4f;
				if (stats.projectileSpeed > 0.0f) stats.projectileSpeed += 4.0f;
				break;
			case ReagentType::Count:
				break;
		}
	}
	return stats;
}

std::string CurseVector::getVectorName() const {
	switch (type) {
		case CurseVectorType::Bottle: return "Bottle";
		case CurseVectorType::Apple: return "Apple";
		case CurseVectorType::Spider: return "Spider";
		case CurseVectorType::Poppet: return "Poppet";
	}
	return "Poppet";
}

std::string CurseVector::getCurseName() const {
	if (type == CurseVectorType::Poppet) return "Poppet Needle";
	if (reagentCount == 0) return "Bare " + getVectorName() + " Hex";

	const ReagentType first = reagents[0];
	bool sameReagent = true;
	for (int i = 1; i < reagentCount; ++i) sameReagent = sameReagent && reagents[i] == first;
	if (sameReagent) {
		std::string name = getReagentDefinition(first).shortName;
		if (reagentCount > 1) name += " x" + std::to_string(reagentCount);
		return name + " " + getVectorName();
	}
	return "Braided " + getVectorName() + " Hex";
}

std::string CurseVector::getPreview() const {
	const SpellStats stats = calculateStats();
	std::ostringstream preview;
	appendPreviewPart(preview, getVectorName());
	if (stats.impactDamage > 0.0f) appendPreviewPart(preview, oneDecimal(stats.impactDamage) + " damage");
	if (stats.radius > 0.2f) appendPreviewPart(preview, oneDecimal(stats.radius) + "m reach");
	if (stats.snareDuration > 0.0f) appendPreviewPart(preview, oneDecimal(stats.snareDuration) + "s snare");
	if (stats.rotDamagePerSecond > 0.0f) {
		appendPreviewPart(preview, oneDecimal(stats.rotDamagePerSecond) + "/s rot for "
			+ oneDecimal(stats.rotDuration) + "s");
	}
	if (stats.fearDuration > 0.0f) appendPreviewPart(preview, oneDecimal(stats.fearDuration) + "s fear");
	if (stats.wardGain > 0.0f) appendPreviewPart(preview, "+" + oneDecimal(stats.wardGain) + " ward");
	const int eyeDoses = getDoseCount(ReagentType::PreservedEye);
	if (eyeDoses > 0) {
		if (type == CurseVectorType::Bottle) {
			appendPreviewPart(preview,
				std::to_string(static_cast<int>(stats.seekingStrength * 100.0f + 0.5f)) + "% homing");
		} else if (type == CurseVectorType::Spider) {
			const float triggerGain = static_cast<float>(eyeDoses) * (0.4f * 0.35f + 0.34f * 0.8f);
			appendPreviewPart(preview, "+" + oneDecimal(triggerGain) + "m hunt trigger");
		} else if (type == CurseVectorType::Apple) {
			appendPreviewPart(preview,
				"+" + oneDecimal(static_cast<float>(eyeDoses) * 0.4f) + "m omen reach");
		}
	}
	if (stats.lingeringDuration > 0.0f) {
		appendPreviewPart(preview, oneDecimal(stats.lingeringDuration) + "s blight");
	}
	if (stats.selfHeal > 0.0f) appendPreviewPart(preview, "+" + oneDecimal(stats.selfHeal) + " health");
	if (stats.hasteDuration > 0.0f) appendPreviewPart(preview, oneDecimal(stats.hasteDuration) + "s haste");
	return preview.str();
}
