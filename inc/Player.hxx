#pragma once

#include "Actor.hxx"
#include "CurseVector.hxx"
#include "InputHandler.hxx"

#include <vector>

class Player : public Actor {
public:
	explicit Player(Vector3 spawnPosition = {0.0f, 0.0f, 18.0f});

	void reset(Vector3 spawnPosition);
	void updatePlayer(const InputHandler::InputState& input, float dt,
		const std::vector<BoundingBox>& obstacles, bool rummaging);

	[[nodiscard]] const Camera3D& camera() const;
	[[nodiscard]] Vector3 viewDirection() const;
	[[nodiscard]] Vector3 flatForward() const;
	[[nodiscard]] bool isGrounded() const;
	[[nodiscard]] float horizontalSpeed() const;

	float takeIncomingDamage(float damage, bool vulnerable);
	void addWard(float amount);
	[[nodiscard]] float ward() const;
	[[nodiscard]] float maxWard() const;
	[[nodiscard]] bool isWardActive() const;
	[[nodiscard]] float wardRegenDelay() const;

	void collect(ReagentType type, int amount = 1);
	[[nodiscard]] int reagentQuantity(ReagentType type) const;
	[[nodiscard]] const ReagentInventory& inventory() const;
	[[nodiscard]] bool consumeFor(const CurseVector& spell);
	[[nodiscard]] bool hasAnyReagents() const;

	[[nodiscard]] bool canCast() const;
	void beginCast(float cooldown);
	void registerHit();
	[[nodiscard]] float castAnimation() const;
	[[nodiscard]] float hitMarker() const;
	[[nodiscard]] float damageFlash() const;
	[[nodiscard]] float movementBob() const;

private:
	void updateCamera(float dt, bool sprinting);
	void moveAndCollide(Vector3 delta, const std::vector<BoundingBox>& obstacles);
	[[nodiscard]] bool collidesAt(Vector3 position, const std::vector<BoundingBox>& obstacles) const;

	Camera3D camera_ {};
	Vector3 spawnPosition_ {};
	ReagentInventory inventory_ {};
	float yaw_ {0.0f};
	float pitch_ {0.0f};
	float ward_ {100.0f};
	float maxWard_ {100.0f};
	float wardRegenDelay_ {0.0f};
	float castCooldown_ {0.0f};
	float castAnimation_ {0.0f};
	float hitMarker_ {0.0f};
	float damageFlash_ {0.0f};
	float invulnerabilityTimer_ {0.0f};
	float bobPhase_ {0.0f};
	float movementBob_ {0.0f};
	bool grounded_ {true};
	bool wardActive_ {false};
};
