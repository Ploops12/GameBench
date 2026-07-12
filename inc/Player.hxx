#pragma once

#include <array>
#include <vector>

#include <raylib.h>
#include "Actor.hxx"
#include "InputHandler.hxx"
#include "Reagent.hxx"

class Player : public Actor {
public:
	Player();

	void update(const InputHandler::InputState& input, float deltaTime, const std::vector<BoundingBox>& obstacles);
	const Camera3D& getCamera() const;
	Vector3 getForward() const;
	bool isRummaging() const;
	bool isSprinting() const;
	bool isWardActive() const;
	float getWard() const;
	float getMaxWard() const;
	float getWardFlash() const;
	void addWard(float amount);
	void heal(float amount);
	void takeDamage(float amount) override;
	std::array<int, REAGENT_COUNT>& getInventory();
	const std::array<int, REAGENT_COUNT>& getInventory() const;

private:
	void updateCamera();
	bool collides(Vector3 candidate, const std::vector<BoundingBox>& obstacles) const;

	Camera3D camera {};
	float yaw {-1.5707963f};
	float pitch {};
	float verticalVelocity {};
	float ward {100.0f};
	float wardDelay {};
	float wardFlash {};
	bool onGround {true};
	bool rummaging {};
	bool sprinting {};
	bool wardActive {};
	std::array<int, REAGENT_COUNT> inventory {0, 0, 0, 0, 0, 0};
};
