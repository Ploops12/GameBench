#pragma once

#include "Actor.hxx"
#include "CurseVector.hxx"
#include <array>
#include <raylib.h>

class Player : public Actor {
public:
	Camera3D camera{};
	Vector3 velocity{};
	std::array<int, REAGENT_COUNT> reagents{};
	SpellDraft draft{};
	float health{100.0f};
	float maxHealth{100.0f};
	float ward{60.0f};
	float maxWard{100.0f};
	float wardCooldown{0.0f};
	float wardFlash{0.0f};
	float castCooldown{0.0f};
	float hurtFlash{0.0f};
	bool wardActive{false};
	bool grounded{false};

	Player();

	Vector3 position() const;
	void setPosition(Vector3 position);
	void giveReagent(ReagentType reagent, int count = 1);
	bool consumeDraftReagents();
	bool hasDraftReagents() const;
	void applyDamage(float amount);
	void updateWard(float dt);
};
