#pragma once

#include <array>
#include <vector>
#include <raylib.h>
#include "Actor.hxx"
#include "CurseVector.hxx"
#include "InputHandler.hxx"

class Player : public Actor {
public:
	Player();
	using Actor::update;

	void update(float dt, const InputHandler::InputState& input,
		const std::vector<BoundingBox>& obstacles, bool rummaging);
	void draw3D() const override {}
	const Camera3D& getCamera() const { return camera; }
	Vector3 getLookDirection() const;
	BoundingBox getBoundsAt(Vector3 value) const;

	void addReagent(ReagentType type, int amount = 1);
	int reagentCount(ReagentType type) const;
	bool selectReagent(ReagentType type);
	void consumeSelectedReagents();
	CurseVector& getSpell() { return spell; }
	const CurseVector& getSpell() const { return spell; }

	void setWardHeld(bool held);
	bool isWardActive() const { return wardActive; }
	float getWard() const { return ward; }
	float getMaxWard() const { return maxWard; }
	void gainWard(float amount);
	void takeDamage(float amount);
	float getDamageFlash() const { return damageFlash; }
	float getCastFlash() const { return castFlash; }
	void triggerCastFlash() { castFlash = 0.18f; }
	bool isGrounded() const { return grounded; }
	void reset();

private:
	Camera3D camera {};
	float yaw {};
	float pitch {};
	bool grounded {true};
	std::array<int, REAGENT_COUNT> inventory {};
	CurseVector spell {};
	float ward {70.0f};
	float maxWard {100.0f};
	bool wardActive {};
	float wardRegenDelay {};
	float damageFlash {};
	float castFlash {};
};
