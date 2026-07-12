#pragma once

#include <raylib.h>
#include "Actor.hxx"
#include "CurseVector.hxx"
#include "InputHandler.hxx"

class Player : public Actor {
public:
	Player();

	void updateCameraFromLook(Vector2 lookDelta);
	Vector3 forward() const;
	Vector3 flatForward() const;
	Vector3 right() const;
	Camera3D& getCamera();
	const Camera3D& getCamera() const;
	void refreshCamera();
	void addReagent(ReagentType type, int amount);
	int totalReagents() const;
	void takeDamage(float amount);
	void gainWard(float amount);
	void updateWard(const InputHandler::InputState& input, float dt);
	bool isWardActive() const;
	float wardRatio() const;
	float healthRatio() const;

	Camera3D camera;
// First-person player: camera, movement state, health, ward, inventory, and active spell selection.
	ReagentInventory inventory{};
	CurseSpell spell{};
	float yaw{};
	float pitch{};
	float eyeHeight{1.55f};
	float groundedTimer{};
	bool grounded{true};
	float ward{55.0f};
	float maxWard{100.0f};
	float wardCooldown{};
	float damageFlash{};
	float castFlash{};
private:
	bool wardActive{};
};
