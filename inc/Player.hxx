#pragma once

#include <array>
#include <vector>
#include <raylib.h>
#include "Actor.hxx"
#include "Animation.hxx"
#include "CurseVector.hxx"
#include "InputHandler.hxx"

class Player : public Actor {
public:
	Player();

	void reset(Vector3 spawnPosition);
	void update(float dt, const InputHandler::InputState& input, const std::vector<BoundingBox>& blockers);
	void draw3D(const Camera3D& camera) const override;
	void drawHands2D(int screenWidth, int screenHeight) const;

	void setHandsSprite(Sprite* sprite);
	void setWardSprite(Sprite* sprite);

	const Camera3D& getCamera() const;
	Vector3 getForward() const;
	Vector3 getRight() const;

	bool isGrounded() const;
	bool isRummaging() const;
	bool isWardActive() const;
	bool isAlive() const;

	float getWard() const;
	float getMaxWard() const;
	float getCastCooldownRemaining() const;
	float getWardCooldownRemaining() const;

	const std::array<int, ReagentTypeCount>& inventory() const;
	void collect(ReagentType type, int amount);

	CurseVector& spellBuilder();
	const CurseVector& spellBuilder() const;
	SpellPreview previewSpell() const;

	bool canCast() const;
	void triggerCastCooldown(float duration);
	bool consumeForSpell(const SpellPreview& spell);
	void addWard(float amount);
	bool applyIncomingDamage(float amount);
private:
	void updateMovement(float dt, const InputHandler::InputState& input, const std::vector<BoundingBox>& blockers);
	void updateWard(float dt, bool wardHeld);
	void resolveCollisions(const std::vector<BoundingBox>& blockers);
	void syncCamera();

	Camera3D camera {};
	float yaw {-90.0f};
	float pitch {0.0f};
	float eyeHeight {1.6f};
	bool grounded {false};
	bool rummaging {false};
	bool wardActive {false};
	float wardCharge {60.0f};
	float maxWard {60.0f};
	float wardCooldownRemaining {0.0f};
	float wardRegenDelay {0.0f};
	float castCooldownRemaining {0.0f};
	float castFlash {0.0f};
	std::array<int, ReagentTypeCount> reagentInventory {};
	CurseVector currentCurse {};
	Sprite* handsSprite {nullptr};
	Sprite* wardSprite {nullptr};
	Animation handsAnimation {};
	Animation wardAnimation {};
};
