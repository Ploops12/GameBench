#pragma once

#include <array>
#include <cstddef>
#include <string_view>

// Reagents are intentionally small, stackable combat resources. Their gameplay
// contribution is compiled by CurseVector; this type only describes inventory
// identity and presentation metadata.
enum class ReagentType : std::size_t {
	GraveSalt,
	ChurchGlass,
	WidowNettle,
	PlagueHoney,
	MothDust,
	BlackWax,
	Count
};

inline constexpr std::size_t kReagentTypeCount = static_cast<std::size_t>(ReagentType::Count);

constexpr bool isValidReagentType(ReagentType type) {
	return static_cast<std::size_t>(type) < kReagentTypeCount;
}

struct ReagentDefinition {
	ReagentType type;
	std::string_view name;
	std::string_view effect;
};

class Reagent {
public:
	explicit constexpr Reagent(ReagentType type) noexcept
		: type_(type) {
	}

	constexpr ReagentType type() const noexcept {
		return type_;
	}

	const ReagentDefinition& definition() const;

	static const ReagentDefinition& definition(ReagentType type);
	static const std::array<ReagentDefinition, kReagentTypeCount>& definitions();

private:
	ReagentType type_;
};

class ReagentInventory {
public:
	int count(ReagentType type) const noexcept;
	const std::array<int, kReagentTypeCount>& counts() const noexcept;

	bool has(ReagentType type, int amount = 1) const noexcept;
	void add(ReagentType type, int amount = 1) noexcept;
	bool remove(ReagentType type, int amount = 1) noexcept;
	void clear() noexcept;
	int total() const noexcept;

private:
	std::array<int, kReagentTypeCount> counts_ {};
};
