#pragma once

#include <array>
#include <raylib.h>
#include "Actor.hxx"
#include "CurseVector.hxx"
#include "InputHandler.hxx"

class Player : public Actor {
public:
	Player();
	void update(float dt, const InputHandler::InputState &input, bool rummaging);
	void drawHands() const;
	Camera3D &getCamera() { return camera; }
	const Camera3D &getCamera() const { return camera; }
	void setPosition(Vector3 value);
	Vector3 forward() const;
	void damage(int value);
	void gainWard(int value);
	void addReagent(ReagentType type, int amount = 1);
	int reagentCount(ReagentType type) const { return inventory[static_cast<int>(type)]; }
	const std::array<int, REAGENT_COUNT> &getInventory() const { return inventory; }
	CurseVector &spell() { return selectedSpell; }
	const CurseVector &spell() const { return selectedSpell; }
	int getHealth() const { return health; }
	int getMaxHealth() const { return maxHealth; }
	int getWard() const { return ward; }
	int getMaxWard() const { return maxWard; }
	bool isWardActive() const { return wardActive; }
	float wardCooldownRatio() const;
	void activateWard();
	void consumeSpellReagents();
	bool isGrounded() const { return grounded; }
	bool isVulnerable() const { return !wardActive; }
	void setRummaging(bool value) { rummaging = value; }
	bool isRummaging() const { return rummaging; }

private:
	Camera3D camera{};
	Vector2 look{0.0f, 0.0f};
	int health{100};
	int maxHealth{100};
	int ward{60};
	int maxWard{60};
	float wardRegen{0.0f};
	float wardTimer{0.0f};
	bool wardActive{false};
	bool grounded{true};
	float verticalSpeed{0.0f};
	bool rummaging{false};
	std::array<int, REAGENT_COUNT> inventory{{2, 1, 1, 0, 0, 0}};
	CurseVector selectedSpell{BaseVector::Bottle};
};
