#pragma once

#include "Reagent.hxx"

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

enum class CurseVectorType {
	Bottle,
	Apple,
	Spider,
	Poppet
};

// All reagent effects resolve into these fields. Delivery code can use the
// vector type to decide whether the payload is thrown, eaten, placed, or fired.
struct SpellStats {
	float damage {0.0f};
	float radius {0.0f};
	float snareDuration {0.0f};
	float poisonDuration {0.0f};
	float fearDuration {0.0f};
	float homingStrength {0.0f};
	float lingeringDuration {0.0f};
	float wardGain {0.0f};

	SpellStats& operator+=(const SpellStats& other) noexcept;
};

// Apple delivery resolves the same assembled payload inward. This keeps the
// reagent rules additive while giving the self-targeted vector a readable,
// useful defensive identity.
struct SelfHexStats {
	float healthRestore {0.0f};
	float wardGain {0.0f};
	float thornGuardDuration {0.0f};
	float thornDamage {0.0f};
	float mendDuration {0.0f};
	float mendPerSecond {0.0f};
	float hasteDuration {0.0f};
	float speedMultiplier {1.0f};
	float waxShellDuration {0.0f};
	float damageReduction {0.0f};
};

SpellStats operator+(SpellStats left, const SpellStats& right) noexcept;

std::string_view curseVectorName(CurseVectorType type) noexcept;
std::string_view curseVectorDelivery(CurseVectorType type) noexcept;
SpellStats baseSpellStats(CurseVectorType type) noexcept;
SpellStats reagentSpellStats(ReagentType type) noexcept;
SelfHexStats selfHexStats(const SpellStats& spell) noexcept;

class CurseVector {
public:
	static constexpr std::size_t kMaxReagents = 3;

	explicit CurseVector(CurseVectorType base = CurseVectorType::Poppet) noexcept;

	CurseVectorType base() const noexcept;
	void setBase(CurseVectorType base) noexcept;

	bool addReagent(ReagentType type) noexcept;
	bool removeReagent(std::size_t index) noexcept;
	bool removeLastReagent() noexcept;
	void clearReagents() noexcept;

	std::size_t reagentCount() const noexcept;
	bool empty() const noexcept;
	bool isPoppetFallback() const noexcept;
	std::optional<ReagentType> reagentAt(std::size_t index) const noexcept;
	const std::array<ReagentType, kMaxReagents>& reagents() const noexcept;

	SpellStats stats() const noexcept;
	bool canConsume(const ReagentInventory& inventory) const noexcept;
	bool consume(ReagentInventory& inventory) const noexcept;

	std::string reagentSummary() const;
	std::string preview() const;
	std::string preview(const ReagentInventory& inventory) const;

private:
	CurseVectorType base_;
	std::array<ReagentType, kMaxReagents> reagents_ {
		ReagentType::Count,
		ReagentType::Count,
		ReagentType::Count
	};
	std::size_t reagentCount_ {0};
};
