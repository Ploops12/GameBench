#pragma once

#include "Actor.hxx"
#include "CurseVector.hxx"
#include "InputHandler.hxx"

#include <array>
#include <vector>

class Player : public Actor {
public:
	Player();

	void update(float deltaTime, const InputHandler::InputState& input, const std::vector<BoundingBox>& obstacles);
	void takeDamage(float amount) override;
	void drawHands(int screenWidth, int screenHeight, bool rummaging, bool casting) const;

	const Camera3D& camera() const;
	Vector3 forward() const;
	Vector3 right() const;
	float yaw() const;
	float pitch() const;
	bool grounded() const;

	void addReagent(ReagentType type, int amount = 1);
	bool consumeReagent(ReagentType type, int amount = 1);
	int reagentQuantity(ReagentType type) const;
	int totalReagents() const;

	float ward() const;
	float maxWard() const;
	bool wardActive() const;
	float wardCooldown() const;
	void addWard(float amount);
	void heal(float amount);
	void setHaste(float duration);
	void setRegeneration(float duration, float rate);
	float hasteTime() const;
	float hurtFlash() const;

private:
	void resolveHorizontalCollision(Vector3& candidate, const std::vector<BoundingBox>& obstacles) const;

	Camera3D viewCamera{};
	float viewYaw{0.0f};
	float viewPitch{0.0f};
	float eyeHeight{1.55f};
	bool onGround{true};
	float wardValue{55.0f};
	float maximumWard{100.0f};
	bool wardShieldActive{false};
	float wardLockout{0.0f};
	float hasteTimer{0.0f};
	float regenerationTimer{0.0f};
	float regenerationRate{0.0f};
	float damageFlash{0.0f};
	std::array<int, REAGENT_COUNT> reagents{};
};
