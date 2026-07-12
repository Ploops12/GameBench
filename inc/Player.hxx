#pragma once

#include "Actor.hxx"
#include "CurseVector.hxx"
#include "InputHandler.hxx"
#include "Reagent.hxx"

class CoatMenu;
class Scenery;

class Player : public Actor {
public:
	Player();

	void update(float dt, const InputHandler::InputState& input, const CoatMenu& coatMenu);
	void syncCamera();
	void applyWorldBounds(float minX, float maxX, float minZ, float maxZ);

	const Camera3D& getCamera() const;
	Vector3 getForward() const;
	Vector3 getRight() const;
	float getEyeHeight() const;
	float getFeetY() const;

	bool isOnGround() const;
	bool isRummaging() const;
	bool isWarding() const;
	bool isSprinting() const;

	float getWard() const;
	float getMaxWard() const;
	float getCastCooldown() const;
	float getCastFlash() const;
	float getRummageDamageScale() const;
	float getWardCooldown() const;

	void addReagent(ReagentType type, int amount);
	const ReagentInventory& getInventory() const;
	bool consumeReagents(const SpellRecipe& recipe);

	void spendWard(float amount);
	void takeHit(float amount);
	void gainWard(float amount);
	void triggerCastFlash();

private:
	void updateMovement(float dt, const InputHandler::InputState& input, const CoatMenu& coatMenu);
	void updateWard(float dt, const InputHandler::InputState& input);

	Camera3D camera;
	ReagentInventory inventory;
	float yaw;
	float pitch;
	float eyeHeight;
	float verticalVelocity;
	float ward;
	float maxWard;
	float wardCooldown;
	float castCooldown;
	float castFlash;
	bool onGround;
	bool rummaging;
	bool warding;
	bool sprinting;
};
