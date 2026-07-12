#include "Reagent.hxx"

#include <algorithm>

namespace {
const std::array<ReagentDefinition, REAGENT_TYPE_COUNT> REAGENT_DEFINITIONS {{
	{ReagentType::BoneShards, "Bone Shards", "BONE", "Impact damage", KEY_FOUR, Color {238, 229, 201, 255}},
	{ReagentType::WidowNettle, "Widow Nettle", "NETTLE", "Thorn snare", KEY_FIVE, Color {138, 220, 92, 255}},
	{ReagentType::Rotcap, "Rotcap", "ROTCAP", "Rot + blight", KEY_SIX, Color {194, 150, 72, 255}},
	{ReagentType::BatWing, "Bat Wing", "WING", "Fear / panic", KEY_SEVEN, Color {211, 134, 224, 255}},
	{ReagentType::WitchGlass, "Witch Glass", "GLASS", "Ward gain", KEY_EIGHT, Color {95, 211, 229, 255}},
	{ReagentType::PreservedEye, "Preserved Eye", "EYE", "Hunt + reach", KEY_NINE, Color {244, 198, 76, 255}}
}};
}

const std::array<ReagentDefinition, REAGENT_TYPE_COUNT>& getReagentDefinitions() {
	return REAGENT_DEFINITIONS;
}

const ReagentDefinition& getReagentDefinition(ReagentType type) {
	const std::size_t index = static_cast<std::size_t>(type);
	if (index >= REAGENT_TYPE_COUNT) return REAGENT_DEFINITIONS[0];
	return REAGENT_DEFINITIONS[index];
}

Reagent::Reagent(ReagentType type, int amount) :
	type(static_cast<std::size_t>(type) < REAGENT_TYPE_COUNT ? type : ReagentType::BoneShards),
	amount(std::max(0, amount)) {
}

ReagentType Reagent::getType() const {
	return type;
}

int Reagent::getAmount() const {
	return amount;
}

int Reagent::take(int requestedAmount) {
	const int taken = std::min(amount, std::max(0, requestedAmount));
	amount -= taken;
	return taken;
}
