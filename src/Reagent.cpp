#include "Reagent.hxx"

#include <algorithm>
#include <array>

namespace {

constexpr std::array<ReagentDefinition, ReagentCount> ReagentDefinitions {
	ReagentDefinition {"Grave Salt", "SALT", "+7 curse harm / +7 apple vigor", {232, 224, 190, 255}},
	ReagentDefinition {"Saint Ash", "ASH", "+15 ward gained", {235, 188, 112, 255}},
	ReagentDefinition {"Black Wax", "WAX", "+2s lingering hex / +0.35m burst", {196, 104, 200, 255}},
	ReagentDefinition {"Widow Nettle", "NETTLE", "+1.4s thorn snare", {105, 232, 132, 255}},
	ReagentDefinition {"Moth Dust", "MOTH", "+1.2s panic / seeking", {128, 204, 255, 255}},
	ReagentDefinition {"Church Glass", "GLASS", "+2.5 rot per second", {255, 117, 98, 255}}
};

}

std::size_t reagentIndex(ReagentType type) {
	const std::size_t index = static_cast<std::size_t>(type);
	return std::min(index, ReagentCount - 1U);
}

const ReagentDefinition& getReagentDefinition(ReagentType type) {
	return ReagentDefinitions[reagentIndex(type)];
}

ReagentType reagentFromIndex(std::size_t index) {
	return static_cast<ReagentType>(std::min(index, ReagentCount - 1U));
}

Reagent::Reagent(ReagentType initialType, int initialAmount)
	: type(initialType), amount(std::max(0, initialAmount)) {
}

ReagentType Reagent::getType() const {
	return type;
}

int Reagent::getAmount() const {
	return amount;
}

void Reagent::addToStack(int addedAmount) {
	amount = std::max(0, amount + addedAmount);
}

bool Reagent::removeFromStack(int removedAmount) {
	if (removedAmount < 0 || removedAmount > amount) {
		return false;
	}
	amount -= removedAmount;
	return true;
}
