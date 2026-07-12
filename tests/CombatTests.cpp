#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <raylib.h>
#include <raymath.h>

#include "Actor.hxx"
#include "Animation.hxx"
#include "CoatMenu.h"
#include "CurseVector.hxx"
#include "Effect.hxx"
#include "Enemy.hxx"
#include "Pickup.hxx"
#include "Player.hxx"
#include "Reagent.hxx"

namespace {

struct TestContext {
	int checks {0};
	int failures {0};

	void check(bool condition, const char* expression, const char* file, int line) {
		++checks;
		if (condition) return;
		++failures;
		std::cerr << file << ':' << line << ": check failed: " << expression << '\n';
	}
};

#define CHECK(context, expression) \
	(context).check(static_cast<bool>(expression), #expression, __FILE__, __LINE__)

bool near(float left, float right, float tolerance = 0.001f) {
	return std::fabs(left - right) <= tolerance;
}

void simulatePlayer(Player& player, const InputHandler::InputState& input, bool rummaging,
		float seconds, const std::vector<BoundingBox>& solids = {}) {
	constexpr float step = 1.0f / 120.0f;
	const int steps = static_cast<int>(seconds / step + 0.5f);
	for (int index = 0; index < steps; ++index) {
		player.updateFromInput(step, input, solids, rummaging);
	}
}

void testReagentsAndSpells(TestContext& context) {
	CHECK(context, REAGENT_TYPE_COUNT == 6);
	const auto& definitions = getReagentDefinitions();
	for (std::size_t index = 0; index < definitions.size(); ++index) {
		CHECK(context, static_cast<std::size_t>(definitions[index].type) == index);
		CHECK(context, definitions[index].selectionKey == KEY_FOUR + static_cast<int>(index));
		CHECK(context, std::string(definitions[index].name).size() > 2);
	}

	const SpellStats bottleBase = CurseVector(CurseVectorType::Bottle).calculateStats();
	const SpellStats appleBase = CurseVector(CurseVectorType::Apple).calculateStats();
	const SpellStats spiderBase = CurseVector(CurseVectorType::Spider).calculateStats();
	const SpellStats poppetBase = CurseVector(CurseVectorType::Poppet).calculateStats();
	CHECK(context, near(bottleBase.impactDamage, 7.0f));
	CHECK(context, near(bottleBase.projectileSpeed, 20.0f));
	CHECK(context, near(appleBase.selfHeal, 6.0f));
	CHECK(context, near(appleBase.hasteDuration, 1.25f));
	CHECK(context, near(spiderBase.radius, 2.0f));
	CHECK(context, near(poppetBase.impactDamage, 9.0f));

	ReagentInventory inventory {};
	inventory.fill(9);
	CurseVector repeated(CurseVectorType::Bottle);
	CHECK(context, repeated.addReagent(ReagentType::BoneShards, inventory));
	const float oneBoneDamage = repeated.calculateStats().impactDamage;
	CHECK(context, repeated.addReagent(ReagentType::BoneShards, inventory));
	const float twoBoneDamage = repeated.calculateStats().impactDamage;
	CHECK(context, repeated.addReagent(ReagentType::BoneShards, inventory));
	const float threeBoneDamage = repeated.calculateStats().impactDamage;
	CHECK(context, near(oneBoneDamage - bottleBase.impactDamage, 16.0f));
	CHECK(context, near(twoBoneDamage - oneBoneDamage, 16.0f));
	CHECK(context, near(threeBoneDamage - twoBoneDamage, 16.0f));
	CHECK(context, !repeated.addReagent(ReagentType::BatWing, inventory));
	CHECK(context, repeated.getReagentCount() == CurseVector::MAX_REAGENTS);
	CHECK(context, repeated.getDoseCount(ReagentType::BoneShards) == 3);
	CHECK(context, repeated.getReagent(-1) == ReagentType::Count);
	CHECK(context, repeated.getReagent(3) == ReagentType::Count);

	const ReagentType types[] = {
		ReagentType::WidowNettle, ReagentType::Rotcap, ReagentType::BatWing,
		ReagentType::WitchGlass, ReagentType::PreservedEye
	};
	for (const ReagentType type : types) {
		CurseVector one(CurseVectorType::Bottle);
		CurseVector two(CurseVectorType::Bottle);
		CHECK(context, one.addReagent(type, inventory));
		CHECK(context, two.addReagent(type, inventory));
		CHECK(context, two.addReagent(type, inventory));
		const SpellStats first = one.calculateStats();
		const SpellStats second = two.calculateStats();
		switch (type) {
		case ReagentType::WidowNettle:
			CHECK(context, near(first.snareDuration, 1.5f));
			CHECK(context, near(second.snareDuration - first.snareDuration, 1.5f));
			break;
		case ReagentType::Rotcap:
			CHECK(context, near(first.rotDamagePerSecond, 3.5f));
			CHECK(context, near(second.rotDamagePerSecond - first.rotDamagePerSecond, 3.5f));
			CHECK(context, near(second.rotDuration - first.rotDuration, 2.75f));
			CHECK(context, near(second.lingeringDuration - first.lingeringDuration, 2.0f));
			break;
		case ReagentType::BatWing:
			CHECK(context, near(first.fearDuration, 1.4f));
			CHECK(context, near(second.fearDuration - first.fearDuration, 1.4f));
			break;
		case ReagentType::WitchGlass:
			CHECK(context, near(first.wardGain, 18.0f));
			CHECK(context, near(second.wardGain - first.wardGain, 18.0f));
			break;
		case ReagentType::PreservedEye:
			CHECK(context, near(first.seekingStrength, 0.34f));
			CHECK(context, near(second.seekingStrength - first.seekingStrength, 0.34f));
			CHECK(context, near(second.radius - first.radius, 0.4f));
			CHECK(context, near(second.projectileSpeed - first.projectileSpeed, 4.0f));
			break;
		default:
			break;
		}
	}

	CurseVector mixed(CurseVectorType::Bottle);
	CHECK(context, mixed.addReagent(ReagentType::BoneShards, inventory));
	CHECK(context, mixed.addReagent(ReagentType::WidowNettle, inventory));
	CHECK(context, mixed.addReagent(ReagentType::WitchGlass, inventory));
	const SpellStats mixedStats = mixed.calculateStats();
	CHECK(context, near(mixedStats.impactDamage, bottleBase.impactDamage + 16.0f));
	CHECK(context, near(mixedStats.snareDuration, 1.5f));
	CHECK(context, near(mixedStats.wardGain, 18.0f));
	CHECK(context, mixed.getCurseName() == "Braided Bottle Hex");
	CHECK(context, mixed.getPreview().find("snare") != std::string::npos);

	ReagentInventory exact {};
	exact[static_cast<std::size_t>(ReagentType::BoneShards)] = 2;
	CurseVector cost(CurseVectorType::Bottle);
	CHECK(context, cost.addReagent(ReagentType::BoneShards, exact));
	CHECK(context, cost.addReagent(ReagentType::BoneShards, exact));
	CHECK(context, !cost.addReagent(ReagentType::BoneShards, exact));
	CHECK(context, cost.consume(exact));
	CHECK(context, exact[0] == 0);
	ReagentInventory insufficient {};
	insufficient[0] = 1;
	CHECK(context, !cost.consume(insufficient));
	CHECK(context, insufficient[0] == 1);

	CurseVector poppet(CurseVectorType::Poppet);
	CHECK(context, !poppet.addReagent(ReagentType::BoneShards, inventory));
	CHECK(context, poppet.canAfford(ReagentInventory {}));
	mixed.setType(CurseVectorType::Poppet);
	CHECK(context, mixed.getReagentCount() == 0);
}

void testPlayer(TestContext& context) {
	InputHandler::InputState forward;
	forward.moveInput.x = 1.0f;
	Player walking;
	Player sprinting;
	Player rummaging;
	Player rummagingSprint;
	walking.reset({0.0f, 0.88f, 0.0f});
	sprinting.reset({0.0f, 0.88f, 0.0f});
	rummaging.reset({0.0f, 0.88f, 0.0f});
	rummagingSprint.reset({0.0f, 0.88f, 0.0f});
	InputHandler::InputState sprint = forward;
	sprint.sprintHeld = true;
	simulatePlayer(walking, forward, false, 1.0f);
	simulatePlayer(sprinting, sprint, false, 1.0f);
	simulatePlayer(rummaging, forward, true, 1.0f);
	simulatePlayer(rummagingSprint, sprint, true, 1.0f);
	const float walkDistance = -walking.getPosition().z;
	const float sprintDistance = -sprinting.getPosition().z;
	const float rummageDistance = -rummaging.getPosition().z;
	CHECK(context, walkDistance > 5.0f);
	CHECK(context, sprintDistance > walkDistance * 1.35f);
	CHECK(context, rummageDistance > 2.0f && rummageDistance < walkDistance * 0.6f);
	CHECK(context, near(rummageDistance, -rummagingSprint.getPosition().z, 0.02f));

	Player diagonal;
	diagonal.reset({0.0f, 0.88f, 0.0f});
	InputHandler::InputState diagonalInput;
	diagonalInput.moveInput = {1.0f, 1.0f, 0.0f};
	simulatePlayer(diagonal, diagonalInput, false, 1.0f);
	const Vector3 diagonalPosition = diagonal.getPosition();
	const float diagonalDistance = std::sqrt(diagonalPosition.x * diagonalPosition.x
		+ diagonalPosition.z * diagonalPosition.z);
	CHECK(context, near(diagonalDistance, walkDistance, 0.08f));

	const std::vector<BoundingBox> wall = {{{-2.0f, 0.0f, -2.0f}, {2.0f, 3.0f, -1.0f}}};
	Player blocked;
	blocked.reset({0.0f, 0.88f, 0.0f});
	simulatePlayer(blocked, forward, false, 2.0f, wall);
	CHECK(context, blocked.getPosition().z > -0.75f);
	Player sliding;
	sliding.reset({0.0f, 0.88f, 0.0f});
	simulatePlayer(sliding, diagonalInput, false, 0.45f, wall);
	CHECK(context, sliding.getPosition().x > 1.0f);
	CHECK(context, sliding.getPosition().z > -0.75f);
	simulatePlayer(sliding, diagonalInput, false, 1.05f, wall);
	CHECK(context, sliding.getPosition().x > 3.0f);

	Player jumper;
	jumper.reset({0.0f, 0.88f, 0.0f});
	InputHandler::InputState jump;
	jump.jumpPressed = true;
	jumper.updateFromInput(1.0f / 120.0f, jump, {}, false);
	float peak = jumper.getPosition().y;
	for (int index = 0; index < 300; ++index) {
		jumper.updateFromInput(1.0f / 120.0f, {}, {}, false);
		peak = std::max(peak, jumper.getPosition().y);
	}
	CHECK(context, peak > 2.0f);
	CHECK(context, near(jumper.getPosition().y, 0.88f, 0.01f));
	CHECK(context, near(jumper.getVelocity().y, 0.0f));

	Player warded;
	warded.reset({0.0f, 0.88f, 0.0f});
	warded.setWardRequested(true, 0.0f);
	CHECK(context, warded.isWardActive());
	warded.takeDamage(20.0f);
	CHECK(context, near(warded.getHealth(), 100.0f));
	CHECK(context, near(warded.getWard(), 50.0f));
	warded.setWardRequested(false, 0.0f);
	warded.takeDamage(10.0f);
	CHECK(context, near(warded.getHealth(), 90.0f));

	Player brokenWard;
	brokenWard.reset({0.0f, 0.88f, 0.0f});
	brokenWard.setWardRequested(true, 5.0f);
	CHECK(context, !brokenWard.isWardActive());
	CHECK(context, brokenWard.isWardBroken());
	CHECK(context, near(brokenWard.getWard(), 0.0f));
	CHECK(context, near(brokenWard.getWardLockout(), 2.5f));
	InputHandler::InputState holdWard;
	holdWard.wardHeld = true;
	simulatePlayer(brokenWard, holdWard, false, 4.0f);
	CHECK(context, brokenWard.getWard() > 5.0f);
	CHECK(context, brokenWard.isWardBroken());
	CHECK(context, !brokenWard.isWardActive());
	brokenWard.updateFromInput(1.0f / 120.0f, {}, {}, false);
	CHECK(context, !brokenWard.isWardBroken());
	brokenWard.updateFromInput(1.0f / 120.0f, holdWard, {}, false);
	CHECK(context, brokenWard.isWardActive());

	Player overflow;
	overflow.reset({0.0f, 0.88f, 0.0f});
	overflow.gainWard(-65.0f);
	overflow.setWardRequested(true, 0.0f);
	overflow.takeDamage(8.0f);
	CHECK(context, near(overflow.getWard(), 0.0f));
	CHECK(context, near(overflow.getHealth(), 97.0f));
	CHECK(context, overflow.isWardBroken());

	Player inventoryPlayer;
	inventoryPlayer.reset({0.0f, 0.88f, 0.0f});
	CHECK(context, inventoryPlayer.addReagent(ReagentType::BoneShards, 98) == 98);
	CHECK(context, inventoryPlayer.addReagent(ReagentType::BoneShards, 2) == 1);
	CHECK(context, inventoryPlayer.getInventory()[0] == 99);
	inventoryPlayer.beginCastCooldown(0.25f);
	CHECK(context, !inventoryPlayer.canCast());
	inventoryPlayer.tickCastCooldown(0.25f);
	CHECK(context, inventoryPlayer.canCast());
}

void testEnemyPickupAndEffects(TestContext& context) {
	Enemy torch(EnemyKind::TorchBearer, {0.0f, 0.9f, -5.0f}, ReagentType::Rotcap);
	const float startZ = torch.getPosition().z;
	torch.updateAI(0.2f, {0.0f, 0.9f, 0.0f}, {});
	CHECK(context, torch.getPosition().z > startZ);

	Enemy afraid(EnemyKind::TorchBearer, {0.0f, 0.9f, -5.0f}, ReagentType::BatWing);
	afraid.applyFear(1.0f);
	afraid.updateAI(0.2f, {0.0f, 0.9f, 0.0f}, {});
	CHECK(context, afraid.getPosition().z < -5.0f);

	Enemy normal(EnemyKind::TorchBearer, {0.0f, 0.9f, -10.0f}, ReagentType::BoneShards);
	Enemy snared(EnemyKind::TorchBearer, {0.0f, 0.9f, -10.0f}, ReagentType::BoneShards);
	snared.applySnare(1.0f);
	normal.updateAI(0.2f, {0.0f, 0.9f, 0.0f}, {});
	snared.updateAI(0.2f, {0.0f, 0.9f, 0.0f}, {});
	CHECK(context, normal.getPosition().z - (-10.0f) > (snared.getPosition().z - (-10.0f)) * 1.8f);

	const std::vector<BoundingBox> wall = {{{-2.0f, 0.0f, -4.0f}, {2.0f, 3.0f, -3.0f}}};
	Enemy blocked(EnemyKind::TorchBearer, {0.0f, 0.9f, -5.0f}, ReagentType::BoneShards);
	for (int index = 0; index < 120; ++index) {
		blocked.updateAI(1.0f / 120.0f, {0.0f, 0.9f, 0.0f}, wall);
	}
	CHECK(context, blocked.getPosition().z < -4.35f);

	Enemy melee(EnemyKind::TorchBearer, {0.0f, 0.9f, -1.9f}, ReagentType::BoneShards);
	EnemyAttackEvent meleeEvent;
	for (int index = 0; index < 240 && !meleeEvent.valid; ++index) {
		meleeEvent = melee.updateAI(1.0f / 120.0f, {0.0f, 0.9f, 0.0f}, {});
	}
	CHECK(context, meleeEvent.valid && !meleeEvent.projectile);
	CHECK(context, near(meleeEvent.damage, 11.0f));

	Enemy hunter(EnemyKind::WitchHunter, {0.0f, 0.95f, -6.0f}, ReagentType::PreservedEye);
	EnemyAttackEvent rangedEvent;
	for (int index = 0; index < 300 && !rangedEvent.valid; ++index) {
		rangedEvent = hunter.updateAI(1.0f / 120.0f, {0.0f, 0.9f, 0.0f}, {});
	}
	CHECK(context, rangedEvent.valid && rangedEvent.projectile);
	CHECK(context, near(rangedEvent.damage, 13.0f));
	CHECK(context, near(Vector3Length(rangedEvent.direction), 1.0f, 0.001f));

	Actor rotting({}, {1.0f, 1.0f, 1.0f}, 100.0f);
	rotting.applyRot(10.0f, 1.0f);
	rotting.update(0.5f);
	CHECK(context, near(rotting.getHealth(), 95.0f));
	rotting.applyRot(2.0f, 3.0f);
	rotting.update(0.5f);
	CHECK(context, near(rotting.getHealth(), 90.0f));
	rotting.update(0.5f);
	CHECK(context, near(rotting.getHealth(), 90.0f));
	rotting.applyRot(2.0f, 1.0f);
	rotting.update(1.0f);
	CHECK(context, near(rotting.getHealth(), 88.0f));

	CHECK(context, !torch.claimDrop());
	torch.takeDamage(1000.0f);
	CHECK(context, torch.claimDrop());
	CHECK(context, !torch.claimDrop());
	Enemy invalidDrop(EnemyKind::TorchBearer, {}, ReagentType::Count);
	CHECK(context, invalidDrop.getDropType() == ReagentType::BoneShards);

	Pickup pickup(ReagentType::Rotcap, 3, {0.0f, 0.5f, 0.0f});
	CHECK(context, pickup.canCollect({0.0f, 0.5f, 0.0f}));
	CHECK(context, !pickup.canCollect({5.0f, 0.5f, 0.0f}));
	CHECK(context, !pickup.canCollect({}, -1.0f));
	CHECK(context, pickup.takeAmount(2) == 2);
	CHECK(context, pickup.getAmount() == 1 && pickup.isActive());
	CHECK(context, pickup.takeAmount(5) == 1);
	CHECK(context, !pickup.isActive());
	CHECK(context, pickup.takeAmount(1) == 0);

	SpellStats hazardStats;
	hazardStats.rotDamagePerSecond = 4.0f;
	Effect hazard(EffectKind::LingeringRot, {}, 2.0f, 1.3f, GREEN, hazardStats);
	CHECK(context, hazard.contains({1.9f, 1.0f, 0.0f}));
	CHECK(context, !hazard.contains({2.1f, 0.0f, 0.0f}));
	CHECK(context, near(hazard.getSpellStats().rotDamagePerSecond, 4.0f));
	hazard.update(0.59f);
	CHECK(context, !hazard.takePulse());
	hazard.update(0.02f);
	CHECK(context, hazard.takePulse());
	CHECK(context, !hazard.takePulse());
	hazard.update(0.6f);
	CHECK(context, hazard.takePulse());
	hazard.update(0.2f);
	CHECK(context, !hazard.isActive());
	Effect burst(EffectKind::Burst, {}, 1.0f, 1.0f, WHITE);
	burst.update(0.7f);
	CHECK(context, !burst.takePulse());
}

void testCoatAndLiveWorld(TestContext& context) {
	Player player;
	player.reset({0.0f, 0.88f, 0.0f});
	player.addReagent(ReagentType::BoneShards, 3);
	player.addReagent(ReagentType::WidowNettle, 2);
	CoatMenu coat;
	InputHandler::InputState input;
	input.coatHeld = true;
	input.coatVectorSelection = 0;
	input.coatReagentSelection = 0;
	coat.update(input, player);
	CHECK(context, coat.isActive());
	CHECK(context, coat.getDraft().getType() == CurseVectorType::Bottle);
	CHECK(context, coat.getDraft().getReagentCount() == 1);
	input.coatVectorSelection = -1;
	coat.update(input, player);
	coat.update(input, player);
	CHECK(context, coat.getDraft().getDoseCount(ReagentType::BoneShards) == 3);
	coat.update(input, player);
	CHECK(context, coat.getDraft().getReagentCount() == 3);
	input.coatRemoveHeld = true;
	coat.update(input, player);
	CHECK(context, coat.getDraft().getReagentCount() == 2);
	input.coatRemoveHeld = false;
	input.coatReagentSelection = -1;
	input.coatUndoPressed = true;
	coat.update(input, player);
	CHECK(context, coat.getDraft().getReagentCount() == 1);
	input.coatUndoPressed = false;
	input.coatReagentSelection = 1;
	coat.update(input, player);
	CHECK(context, coat.getDraft().getReagentCount() == 2);
	input = {};
	coat.update(input, player);
	CHECK(context, !coat.isActive());
	CHECK(context, coat.wasCommitted());
	CHECK(context, !coat.wasCommitted());
	CHECK(context, player.getPreparedSpell().getReagentCount() == 2);
	CHECK(context, player.getInventory()[0] == 3);

	ReagentInventory abundant {};
	abundant[0] = 2;
	abundant[1] = 1;
	CurseVector mixed(CurseVectorType::Bottle);
	mixed.addReagent(ReagentType::BoneShards, abundant);
	mixed.addReagent(ReagentType::WidowNettle, abundant);
	mixed.addReagent(ReagentType::BoneShards, abundant);
	Player trimmedPlayer;
	trimmedPlayer.reset({0.0f, 0.88f, 0.0f});
	trimmedPlayer.addReagent(ReagentType::WidowNettle, 1);
	trimmedPlayer.setPreparedSpell(mixed);
	CoatMenu trimmedCoat;
	InputHandler::InputState open;
	open.coatHeld = true;
	trimmedCoat.update(open, trimmedPlayer);
	CHECK(context, trimmedCoat.getDraft().getReagentCount() == 1);
	CHECK(context, trimmedCoat.getDraft().getReagent(0) == ReagentType::WidowNettle);
	trimmedCoat.reset();
	CHECK(context, !trimmedCoat.isActive());
	CHECK(context, trimmedCoat.getDraft().getType() == CurseVectorType::Poppet);

	Player movingPlayer;
	movingPlayer.reset({0.0f, 0.88f, 0.0f});
	Enemy movingEnemy(EnemyKind::TorchBearer, {0.0f, 0.9f, -12.0f}, ReagentType::Rotcap);
	Pickup movingPickup(ReagentType::Rotcap, 1, {4.0f, 0.5f, 0.0f});
	Effect tickingEffect(EffectKind::LingeringRot, {4.0f, 0.0f, 0.0f}, 1.0f, 2.0f, GREEN);
	CoatMenu liveCoat;
	InputHandler::InputState liveInput;
	liveInput.coatHeld = true;
	liveInput.moveInput.x = 1.0f;
	liveInput.sprintHeld = true;
	liveInput.wardHeld = true;
	const float enemyStart = movingEnemy.getPosition().z;
	const float effectStart = tickingEffect.getRemainingTime();
	for (int index = 0; index < 120; ++index) {
		liveCoat.update(liveInput, movingPlayer);
		movingPlayer.updateFromInput(1.0f / 120.0f, liveInput, {}, liveCoat.isActive());
		movingEnemy.updateAI(1.0f / 120.0f, movingPlayer.getPosition(), {});
		movingPickup.update(1.0f / 120.0f);
		tickingEffect.update(1.0f / 120.0f);
	}
	CHECK(context, liveCoat.isActive());
	CHECK(context, -movingPlayer.getPosition().z > 2.0f);
	CHECK(context, -movingPlayer.getPosition().z < 3.1f);
	CHECK(context, movingEnemy.getPosition().z > enemyStart);
	CHECK(context, movingPickup.getPosition().y != 0.5f);
	CHECK(context, tickingEffect.getRemainingTime() < effectStart - 0.9f);
	CHECK(context, movingPlayer.getWard() < 70.0f);
}

void testAnimation(TestContext& context) {
	Animation animation(Sprite {}, 0, 1, 10.0f, false);
	CHECK(context, animation.getCurrentFrame() == 0);
	animation.update(0.2f);
	CHECK(context, animation.isFinished());
	animation.reset();
	CHECK(context, !animation.isFinished());
}

} // namespace

int main() {
	TestContext context;
	testReagentsAndSpells(context);
	testPlayer(context);
	testEnemyPickupAndEffects(context);
	testCoatAndLiveWorld(context);
	testAnimation(context);

	if (context.failures == 0) {
		std::cout << "All " << context.checks << " combat-system checks passed.\n";
		return 0;
	}
	std::cerr << context.failures << " of " << context.checks << " checks failed.\n";
	return 1;
}
