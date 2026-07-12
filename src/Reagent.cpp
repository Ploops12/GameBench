#include <array>
#include "Reagent.hxx"

namespace {
	const std::array<ReagentDescriptor, ReagentTypeCount> kDescriptors {{
		{ReagentType::GraveSalt, "Grave Salt", "Salt", "damage", Color {235, 233, 220, 255}, KEY_Q},
		{ReagentType::SaintAsh, "Saint Ash", "Ash", "ward gain", Color {244, 211, 94, 255}, KEY_W},
		{ReagentType::WidowNettle, "Widow Nettle", "Nettle", "snare", Color {86, 214, 110, 255}, KEY_E},
		{ReagentType::BellBronze, "Bell Bronze", "Bronze", "seeking", Color {227, 135, 58, 255}, KEY_A},
		{ReagentType::MothDust, "Moth Dust", "Moth", "panic", Color {170, 120, 240, 255}, KEY_S},
		{ReagentType::BogRot, "Bog Rot", "Rot", "rot", Color {94, 196, 132, 255}, KEY_D}
	}};
}

Reagent::Reagent(ReagentType type)
	: type(type) {
}

ReagentType Reagent::getType() const {
	return type;
}

const ReagentDescriptor& Reagent::descriptor() const {
	return GetReagentDescriptor(type);
}

const ReagentDescriptor& GetReagentDescriptor(ReagentType type) {
	return kDescriptors[static_cast<std::size_t>(type)];
}

const ReagentDescriptor& GetReagentDescriptor(std::size_t index) {
	return kDescriptors[index];
}
