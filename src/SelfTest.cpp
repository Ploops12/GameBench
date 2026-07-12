#include "SelfTest.hxx"
#include "CurseVector.hxx"
#include "Effect.hxx"
#include "Enemy.hxx"
#include "Player.hxx"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {
bool closeEnough(float actual, float expected, float tolerance = 0.001f) {
	return std::fabs(actual - expected) <= tolerance;
}

class TestRunner {
public:
	void check(bool condition, const std::string& name) {
		if (condition) return;
		passed = false;
		std::cerr << "SELF-TEST FAILED: " << name << '\n';
	}

	bool result() const { return passed; }

private:
	bool passed {true};
};
}

bool runSelfTests() {
	TestRunner test;
	test.check(REAGENT_COUNT == 6, "exactly six reagent types");

	CurseVector curse(BaseVector::Bottle);
	test.check(curse.addReagent(ReagentType::GraveSalt), "first reagent selection");
	test.check(curse.addReagent(ReagentType::GraveSalt), "repeated reagent selection");
	test.check(curse.addReagent(ReagentType::GraveSalt), "third reagent selection");
	test.check(!curse.addReagent(ReagentType::SaintAsh), "three-reagent capacity");
	SpellStats stats = curse.compose();
	test.check(closeEnough(stats.damage, 52.0f), "repeated grave salt stacks ordinary damage");

	curse = CurseVector(BaseVector::Spider);
	curse.addReagent(ReagentType::WidowNettle);
	curse.addReagent(ReagentType::WidowNettle);
	stats = curse.compose();
	test.check(closeEnough(stats.snareDuration, 2.5f), "widow nettle stacks snare");

	curse = CurseVector(BaseVector::Bottle);
	curse.addReagent(ReagentType::BogMarrow);
	curse.addReagent(ReagentType::MothDust);
	curse.addReagent(ReagentType::ThornHeart);
	stats = curse.compose();
	test.check(closeEnough(stats.rotDamagePerSecond, 4.0f) && closeEnough(stats.fearDuration, 1.0f),
		"rot and panic compose independently");
	test.check(stats.homingStrength == 1 && closeEnough(stats.splashRadius, 1.9f) && closeEnough(stats.lingeringDuration, 3.5f),
		"seeking splash and lingering compose independently");

	curse = CurseVector(BaseVector::Apple);
	curse.addReagent(ReagentType::SaintAsh);
	curse.addReagent(ReagentType::SaintAsh);
	stats = curse.compose();
	test.check(closeEnough(stats.healAmount, 18.0f) && closeEnough(stats.wardGain, 38.0f),
		"apple self effect and saint ash ward stack");

	Player inventoryPlayer;
	inventoryPlayer.addReagent(ReagentType::GraveSalt, 2);
	test.check(inventoryPlayer.selectReagent(ReagentType::GraveSalt), "inventory selects available reagent");
	test.check(inventoryPlayer.selectReagent(ReagentType::GraveSalt), "inventory selects available repeated reagent");
	test.check(!inventoryPlayer.selectReagent(ReagentType::GraveSalt), "inventory rejects unavailable reagent");
	inventoryPlayer.consumeSelectedReagents();
	test.check(inventoryPlayer.reagentCount(ReagentType::GraveSalt) == 0 && inventoryPlayer.getSpell().getReagentCount() == 0,
		"casting consumption clears selected reagents");

	Player wardPlayer;
	wardPlayer.setWardHeld(true);
	wardPlayer.takeDamage(20.0f);
	test.check(closeEnough(wardPlayer.getHealth(), 96.0f) && closeEnough(wardPlayer.getWard(), 38.0f),
		"active ward absorbs damage and spends resource");

	InputHandler::InputState movement;
	movement.moveInput.x = 1.0f;
	Player normalMovement;
	const float normalStart = normalMovement.getPosition().z;
	normalMovement.update(0.1f, movement, {}, false);
	const float normalDistance = normalStart - normalMovement.getPosition().z;
	Player rummageMovement;
	const float rummageStart = rummageMovement.getPosition().z;
	rummageMovement.update(0.1f, movement, {}, true);
	const float rummageDistance = rummageStart - rummageMovement.getPosition().z;
	test.check(normalDistance > rummageDistance && rummageDistance > 0.0f, "rummaging slows without immobilizing");

	movement.jumpPressed = true;
	Player normalJump;
	normalJump.update(0.1f, movement, {}, false);
	Player rummageJump;
	rummageJump.update(0.1f, movement, {}, true);
	test.check(normalJump.getPosition().y > 0.9f && closeEnough(rummageJump.getPosition().y, 0.9f),
		"rummaging disables jumping");

	Player collisionPlayer;
	const BoundingBox wall{Vector3{-1.0f, 0.0f, 7.0f}, Vector3{1.0f, 2.0f, 7.4f}};
	collisionPlayer.update(0.1f, movement, std::vector<BoundingBox>{wall}, false);
	test.check(closeEnough(collisionPlayer.getPosition().z, 8.0f), "player resolves scenery collision");

	Enemy enemy(Vector3{0.0f, 0.0f, 0.0f});
	const BoundingBox enemyWall{Vector3{-1.0f, 0.0f, -1.6f}, Vector3{1.0f, 2.0f, -1.0f}};
	enemy.update(1.0f, Vector3{0.0f, 0.0f, -10.0f}, std::vector<BoundingBox>{enemyWall});
	test.check(closeEnough(enemy.getPosition().z, 0.0f), "enemy resolves scenery collision");

	Effect projectile(EffectDelivery::Projectile, Vector3{0.0f, 1.0f, 0.0f}, Vector3{0.0f, 0.0f, -10.0f}, SpellStats{});
	projectile.update(0.1f);
	test.check(projectile.getPosition().z < -0.9f && projectile.getVelocity().y < 0.0f, "bottle projectile advances with gravity");

	if (test.result()) std::cout << "All combat self-tests passed.\n";
	return test.result();
}
