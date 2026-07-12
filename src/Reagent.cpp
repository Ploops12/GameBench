#include "Reagent.hxx"

namespace {

constexpr std::array<ReagentDefinition, kReagentTypeCount> kDefinitions {{
	{ReagentType::GraveSalt, "Grave Salt", "+ damage"},
	{ReagentType::ChurchGlass, "Church Glass", "+ impact radius"},
	{ReagentType::WidowNettle, "Widow Nettle", "+ thorn snare"},
	{ReagentType::PlagueHoney, "Plague Honey", "+ poison rot"},
	{ReagentType::MothDust, "Moth Dust", "+ fear and seeking"},
	{ReagentType::BlackWax, "Black Wax", "+ lingering hazard"}
}};

constexpr std::size_t reagentIndex(ReagentType type) {
	return static_cast<std::size_t>(type);
}

} // namespace

const ReagentDefinition& Reagent::definition() const {
	return definition(type_);
}

const ReagentDefinition& Reagent::definition(ReagentType type) {
	return definitions().at(reagentIndex(type));
}

const std::array<ReagentDefinition, kReagentTypeCount>& Reagent::definitions() {
	return kDefinitions;
}

int ReagentInventory::count(ReagentType type) const noexcept {
	if (!isValidReagentType(type)) {
		return 0;
	}

	return counts_[reagentIndex(type)];
}

const std::array<int, kReagentTypeCount>& ReagentInventory::counts() const noexcept {
	return counts_;
}

bool ReagentInventory::has(ReagentType type, int amount) const noexcept {
	return amount > 0 && count(type) >= amount;
}

void ReagentInventory::add(ReagentType type, int amount) noexcept {
	if (!isValidReagentType(type) || amount <= 0) {
		return;
	}

	counts_[reagentIndex(type)] += amount;
}

bool ReagentInventory::remove(ReagentType type, int amount) noexcept {
	if (!has(type, amount)) {
		return false;
	}

	counts_[reagentIndex(type)] -= amount;
	return true;
}

void ReagentInventory::clear() noexcept {
	counts_.fill(0);
}

int ReagentInventory::total() const noexcept {
	int result = 0;
	for (int amount : counts_) {
		result += amount;
	}
	return result;
}
