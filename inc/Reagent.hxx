#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <raylib.h>
#include <string_view>

enum class ReagentType : std::uint8_t {
	BoneSplinters,
	GraveSalt,
	WidowNettle,
	Rotcap,
	MothDust,
	SaintAsh,
	Count
};

constexpr std::size_t REAGENT_TYPE_COUNT = static_cast<std::size_t>(ReagentType::Count);
using ReagentInventory = std::array<int, REAGENT_TYPE_COUNT>;

struct ReagentDefinition {
	ReagentType type;
	std::string_view name;
	std::string_view shortName;
	std::string_view effect;
	int selectionKey;
	std::string_view keyLabel;
	Color color;
};

[[nodiscard]] constexpr std::size_t ReagentIndex(ReagentType type) {
	return static_cast<std::size_t>(type);
}

[[nodiscard]] const std::array<ReagentDefinition, REAGENT_TYPE_COUNT>& GetReagentDefinitions();
[[nodiscard]] const ReagentDefinition& GetReagentDefinition(ReagentType type);

class Reagent {
public:
	explicit Reagent(ReagentType type);
	[[nodiscard]] ReagentType type() const;
	[[nodiscard]] const ReagentDefinition& definition() const;

private:
	ReagentType type_;
};
