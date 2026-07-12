#pragma once

#include <vector>
#include <raylib.h>

#include "Actor.hxx"
#include "CurseVector.hxx"
#include "InputHandler.hxx"

class Player : public Actor {
	// First-person movement, camera, actions, stats, coat inventory, and ward.
public:
	Player();

	void reset(Vector3 spawnPosition);
	void updateFromInput(float deltaTime, const InputHandler::InputState& input,
		const std::vector<BoundingBox>& solidBounds, bool rummaging);
	float takeDamage(float damage) override;
	void draw3D(const Camera3D& camera) const override;

	const Camera3D& getCamera() const;
	Vector3 getEyePosition() const;
	Vector3 getLookDirection() const;
	Vector3 getFlatForward() const;

	int addReagent(ReagentType type, int amount);
	const ReagentInventory& getInventory() const;
	ReagentInventory& getInventory();
	const CurseVector& getPreparedSpell() const;
	void setPreparedSpell(const CurseVector& spell);
	bool consumePreparedReagents();

	void setWardRequested(bool requested, float deltaTime);
	void gainWard(float amount);
	float getWard() const;
	float getMaxWard() const;
	float getWardRatio() const;
	bool isWardActive() const;
	bool isWardBroken() const;
	float getWardLockout() const;
	float getWardFlash() const;

	void applyHaste(float duration);
	float getHasteTime() const;
	float getDamageFlash() const;
	void tickCastCooldown(float deltaTime);
	bool canCast() const;
	void beginCastCooldown(float duration);

private:
	bool collidesAt(Vector3 candidate, const std::vector<BoundingBox>& solidBounds) const;
	void updateCamera(float deltaTime, Vector2 lookInput, float moveAmount, bool sprinting = false);

	Camera3D camera {};
	float yaw {0.0f};
	float pitch {0.0f};
	bool grounded {true};
	ReagentInventory inventory {};
	CurseVector preparedSpell;
	float ward {70.0f};
	float maxWard {100.0f};
	bool wardActive {false};
	bool wardBroken {false};
	float wardRegenDelay {0.0f};
	float wardLockout {0.0f};
	float wardFlash {0.0f};
	float damageFlash {0.0f};
	float hasteTime {0.0f};
	float castCooldown {0.0f};
	float bobPhase {0.0f};
};
