#pragma once

#include <raylib.h>
#include "Actor.hxx"
#include "CurseVector.hxx"
#include "InputHandler.hxx"
#include "Reagent.hxx"

#include <array>
#include <vector>

class Player : public Actor {
public:
	Player();

	void update(const InputHandler::InputState& input, float deltaTime,
		const std::vector<BoundingBox>& colliders, bool rummaging);
	void takeDamage(float amount) override;
	const Camera3D& getCamera() const;
	Vector3 getEyePosition() const;
	Vector3 getForward() const;
	Vector3 getRight() const;
	bool canCast() const;
	void beginCast(float cooldown);
	float getCastCooldown() const;
	float getWard() const;
	float getMaxWard() const;
	bool isWardActive() const;
	float getDamageFlash() const;
	void addWard(float amount);
	void addReagent(ReagentType type, int amount = 1);
	const std::array<int, ReagentCount>& getInventory() const;
	bool consumeReagents(const CurseVector& spell);
	void drawHands(float time, const SpellStats& spell, bool rummaging) const;

private:
	bool collidesWithWorld(Vector3 candidatePosition, const std::vector<BoundingBox>& colliders) const;
	void updateCamera();

	Camera3D camera {};
	float yaw {-1.5707963f};
	float pitch {-0.10f};
	float verticalVelocity {0.0f};
	float castCooldown {0.0f};
	float ward {100.0f};
	float maxWard {100.0f};
	float wardBreakCooldown {0.0f};
	float damageFlash {0.0f};
	bool wardActive {false};
	bool grounded {true};
	std::array<int, ReagentCount> inventory {};
};
