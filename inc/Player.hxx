#pragma once

#include <raylib.h>
#include <vector>

#include "Actor.hxx"
#include "CurseVector.hxx"
#include "InputHandler.hxx"

class Player : public Actor {
public:
	using Actor::update;

	Player();

	void update(float deltaTime, const InputHandler::InputState& input,
		const std::vector<BoundingBox>& solidBounds, bool rummaging);
	void draw(const Camera3D& camera) const override;
	void takeDamage(float amount) override;

	const Camera3D& getCamera() const;
	Vector3 getForward() const;
	Vector3 getCastOrigin() const;
	bool isGrounded() const;
	bool isWardActive() const;
	float getWard() const;
	float getMaximumWard() const;
	float getDamageFlash() const;
	float getCastCooldown() const;
	float getThornGuardTime() const;
	float getThornRetaliationDamage() const;
	float getThornSnareDuration() const;
	float getMendTime() const;
	float getPanicStepTime() const;
	float getWaxShellTime() const;
	void gainWard(float amount);
	void applySelfHex(const SelfHexStats& hex);
	bool beginCast(float cooldown);

private:
	void updateCamera();
	bool collidesWithWorld(Vector3 candidate, const std::vector<BoundingBox>& solidBounds) const;

	Camera3D camera {};
	float yaw {-PI*0.5f};
	float pitch {0.0f};
	float verticalVelocity {0.0f};
	float ward {100.0f};
	float wardRechargeDelay {0.0f};
	float damageFlash {0.0f};
	float castCooldown {0.0f};
	float thornGuardTimer {0.0f};
	float thornRetaliationDamage {0.0f};
	float thornSnareDuration {0.0f};
	float mendTimer {0.0f};
	float mendPerSecond {0.0f};
	float panicStepTimer {0.0f};
	float panicStepMultiplier {1.0f};
	float waxShellTimer {0.0f};
	float waxShellReduction {0.0f};
	bool grounded {true};
	bool wardActive {false};
};
