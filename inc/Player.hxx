#pragma once

#include <array>
#include <vector>
#include <raylib.h>
#include "Actor.hxx"
#include "InputHandler.hxx"

class Player : public Actor {
public:
	using Object::Update;

	Player();
	void Update(const InputHandler::InputState& input, float dt, bool rummaging, const std::vector<BoundingBox>& obstacles);
	void DrawWardOverlay() const;
	Camera3D& GetCamera() { return camera; }
	const Camera3D& GetCamera() const { return camera; }
	Vector3 GetForward() const;
	float GetHealth() const { return health; }
	float GetMaxHealth() const { return maxHealth; }
	float GetWard() const { return ward; }
	float GetMaxWard() const { return maxWard; }
	bool IsWardActive() const { return wardActive; }
	float GetWardCooldown() const { return wardCooldown; }
	void ActivateWard();
	float TakeDamage(float amount);
	void Heal(float amount);
	void GainWard(float amount);
	void SetPosition(Vector3 value);
	bool IsAlive() const { return health > 0.0f; }

private:
	Camera3D camera{};
	float yaw{-90.0f};
	float pitch{0.0f};
	float health{100.0f};
	float maxHealth{100.0f};
	float ward{55.0f};
	float maxWard{100.0f};
	float verticalVelocity{0.0f};
	float wardTimer{0.0f};
	float wardCooldown{0.0f};
	bool onGround{true};
	bool wardActive{false};
};
