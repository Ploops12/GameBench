#include "Reagent.hxx"

#include <stdexcept>

const std::array<ReagentDefinition, REAGENT_TYPE_COUNT>& GetReagentDefinitions() {
	static const std::array<ReagentDefinition, REAGENT_TYPE_COUNT> definitions {{
		{ReagentType::BoneSplinters, "Bone Splinters", "BONE", "+14 curse damage", KEY_Z, "Z", {255, 226, 166, 255}},
		{ReagentType::GraveSalt, "Grave Salt", "SALT", "+0.9 blast radius", KEY_X, "X", {183, 231, 255, 255}},
		{ReagentType::WidowNettle, "Widow Nettle", "NETTLE", "+snare and duration", KEY_C, "C", {150, 255, 128, 255}},
		{ReagentType::Rotcap, "Rotcap", "ROT", "+rot and lingering mire", KEY_V, "V", {203, 255, 75, 255}},
		{ReagentType::MothDust, "Moth Dust", "MOTH", "+seeking and panic", KEY_B, "B", {255, 142, 223, 255}},
		{ReagentType::SaintAsh, "Saint Ash", "ASH", "+18 ward on delivery", KEY_N, "N", {120, 229, 255, 255}}
	}};
	return definitions;
}

const ReagentDefinition& GetReagentDefinition(ReagentType type) {
	const std::size_t index = ReagentIndex(type);
	if (index >= REAGENT_TYPE_COUNT) {
		throw std::out_of_range("Invalid reagent type");
	}
	return GetReagentDefinitions()[index];
}

Reagent::Reagent(ReagentType type)
	: type_(type) {
}

ReagentType Reagent::type() const {
	return type_;
}

const ReagentDefinition& Reagent::definition() const {
	return GetReagentDefinition(type_);
}
