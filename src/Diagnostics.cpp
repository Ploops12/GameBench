#include "Game.h"

#include <raymath.h>

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

namespace {
class CheckLog {
public:
	void expect(bool condition, const std::string& label) {
		++total_;
		if (!condition) {
			failures_.push_back(label);
		}
	}

	void expectNear(float actual, float expected, float tolerance, const std::string& label) {
		expect(std::fabs(actual - expected) <= tolerance, label);
	}

	[[nodiscard]] bool passed() const {
		return failures_.empty();
	}

	[[nodiscard]] std::string report() const {
		std::ostringstream output;
		output << "combat-slice checks: " << (total_ - static_cast<int>(failures_.size())) << "/" << total_;
		output << (passed() ? " PASS" : " FAIL");
		for (const std::string& failure : failures_) {
			output << "\n  - " << failure;
		}
		return output.str();
	}

private:
	int total_ {0};
	std::vector<std::string> failures_;
};

void AdvancePlayer(Player& player, const InputHandler::InputState& input, float seconds,
	bool rummaging = false, const std::vector<BoundingBox>& obstacles = {}) {
	constexpr float step = 1.0f / 120.0f;
	float elapsed = 0.0f;
	while (elapsed + step <= seconds) {
		player.updatePlayer(input, step, obstacles, rummaging);
		elapsed += step;
	}
}

SpellStats StackedStats(ReagentType reagent, int copies) {
	CurseVector spell;
	spell.selectBase(BaseVectorType::Bottle);
	for (int i = 0; i < copies; ++i) {
		const bool added = spell.addReagent(reagent);
		(void)added;
	}
	return spell.stats();
}
}

bool Game::runDeterministicChecks(std::string& report) {
	CheckLog checks;
	checks.expect(GetReagentDefinitions().size() == 6, "exactly six reagent definitions exist");

	const SpellStats oneBone = StackedStats(ReagentType::BoneSplinters, 1);
	const SpellStats twoBone = StackedStats(ReagentType::BoneSplinters, 2);
	const SpellStats oneSalt = StackedStats(ReagentType::GraveSalt, 1);
	const SpellStats twoSalt = StackedStats(ReagentType::GraveSalt, 2);
	const SpellStats oneNettle = StackedStats(ReagentType::WidowNettle, 1);
	const SpellStats twoNettle = StackedStats(ReagentType::WidowNettle, 2);
	const SpellStats oneRotcap = StackedStats(ReagentType::Rotcap, 1);
	const SpellStats twoRotcap = StackedStats(ReagentType::Rotcap, 2);
	const SpellStats oneMoth = StackedStats(ReagentType::MothDust, 1);
	const SpellStats twoMoth = StackedStats(ReagentType::MothDust, 2);
	const SpellStats oneAsh = StackedStats(ReagentType::SaintAsh, 1);
	const SpellStats twoAsh = StackedStats(ReagentType::SaintAsh, 2);
	checks.expect(twoBone.damage > oneBone.damage, "repeated Bone Splinters increase damage");
	checks.expect(twoSalt.radius > oneSalt.radius, "repeated Grave Salt increases radius");
	checks.expect(twoNettle.snareStrength > oneNettle.snareStrength && twoNettle.duration > oneNettle.duration,
		"repeated Widow Nettle strengthens snare and duration");
	checks.expect(twoRotcap.poisonDps > oneRotcap.poisonDps && twoRotcap.duration > oneRotcap.duration,
		"repeated Rotcap strengthens rot and duration");
	checks.expect(twoMoth.homingStrength > oneMoth.homingStrength && twoMoth.fearDuration > oneMoth.fearDuration,
		"repeated Moth Dust strengthens seeking and panic");
	checks.expect(twoAsh.wardGain > oneAsh.wardGain, "repeated Saint Ash increases ward return");

	CurseVector cappedSpell;
	cappedSpell.selectBase(BaseVectorType::Bottle);
	checks.expect(cappedSpell.addReagent(ReagentType::BoneSplinters), "first reagent slot accepts a selection");
	checks.expect(cappedSpell.addReagent(ReagentType::GraveSalt), "second reagent slot accepts a selection");
	checks.expect(cappedSpell.addReagent(ReagentType::Rotcap), "third reagent slot accepts a selection");
	checks.expect(!cappedSpell.addReagent(ReagentType::SaintAsh), "a fourth reagent selection is rejected");
	ReagentInventory sufficientInventory {};
	sufficientInventory[ReagentIndex(ReagentType::BoneSplinters)] = 1;
	sufficientInventory[ReagentIndex(ReagentType::GraveSalt)] = 1;
	sufficientInventory[ReagentIndex(ReagentType::Rotcap)] = 1;
	checks.expect(cappedSpell.canConsume(sufficientInventory), "a selected spell accepts sufficient stack inventory");
	sufficientInventory[ReagentIndex(ReagentType::Rotcap)] = 0;
	checks.expect(!cappedSpell.canConsume(sufficientInventory), "a selected spell rejects insufficient stack inventory");
	cappedSpell.selectBase(BaseVectorType::Poppet);
	checks.expect(cappedSpell.isFallback() && cappedSpell.reagents().empty(), "Poppet is always available and clears reagent slots");

	CurseVector bottleBase;
	bottleBase.selectBase(BaseVectorType::Bottle);
	CurseVector appleBase;
	appleBase.selectBase(BaseVectorType::Apple);
	CurseVector spiderBase;
	spiderBase.selectBase(BaseVectorType::Spider);
	checks.expect(bottleBase.stats().projectileSpeed > 0.0f && bottleBase.stats().damage > 0.0f,
		"Bottle has thrown-projectile delivery stats");
	checks.expect(appleBase.stats().heal > 0.0f && appleBase.stats().duration > 0.0f,
		"Apple has self-targeted healing aura stats");
	checks.expect(spiderBase.stats().damage > 0.0f && spiderBase.stats().radius > 0.0f,
		"Spider has proximity-trap delivery stats");

	InputHandler::InputState forwardInput;
	forwardInput.moveInput.x = 1.0f;
	Player walking({0.0f, 0.0f, 0.0f});
	AdvancePlayer(walking, forwardInput, 1.0f);
	Player sprinting({0.0f, 0.0f, 0.0f});
	InputHandler::InputState sprintInput = forwardInput;
	sprintInput.sprintHeld = true;
	AdvancePlayer(sprinting, sprintInput, 1.0f);
	Player rummaging({0.0f, 0.0f, 0.0f});
	AdvancePlayer(rummaging, sprintInput, 1.0f, true);
	const float walkDistance = -walking.position().z;
	const float sprintDistance = -sprinting.position().z;
	const float rummageDistance = -rummaging.position().z;
	checks.expect(walkDistance > 4.5f, "first-person walking remains responsive");
	checks.expect(sprintDistance > walkDistance, "sprint is faster than walking");
	checks.expect(rummageDistance > 1.0f && rummageDistance < walkDistance,
		"rummaging slows movement without immobilizing the player");

	Player jumping({0.0f, 0.0f, 0.0f});
	InputHandler::InputState jumpInput;
	jumpInput.jumpPressed = true;
	jumping.updatePlayer(jumpInput, 1.0f / 60.0f, {}, false);
	checks.expect(jumping.position().y > 0.05f && !jumping.isGrounded(), "jumping creates upward movement");
	Player rummageJump({0.0f, 0.0f, 0.0f});
	rummageJump.updatePlayer(jumpInput, 1.0f / 60.0f, {}, true);
	checks.expect(rummageJump.position().y == 0.0f && rummageJump.isGrounded(), "rummaging disables jumping");

	Player colliding({0.0f, 0.0f, 0.0f});
	const std::vector<BoundingBox> wall {{{-2.0f, 0.0f, -2.0f}, {2.0f, 2.0f, -1.0f}}};
	AdvancePlayer(colliding, forwardInput, 1.0f, false, wall);
	checks.expect(colliding.position().z > -0.75f, "player collision prevents passing through solid scenery");

	Player shielded({0.0f, 0.0f, 0.0f});
	InputHandler::InputState wardInput;
	wardInput.wardHeld = true;
	shielded.updatePlayer(wardInput, 0.1f, {}, false);
	const float wardBeforeHit = shielded.ward();
	const float healthBeforeHit = shielded.health();
	const float shieldedDamage = shielded.takeIncomingDamage(10.0f, false);
	checks.expect(shielded.isWardActive() && shielded.ward() < wardBeforeHit,
		"active ward visibly consumes its resource when hit");
	checks.expectNear(shielded.health(), healthBeforeHit, 0.001f, "active ward absorbs incoming health damage");
	checks.expectNear(shieldedDamage, 0.0f, 0.001f, "fully absorbed ward hit reports no health loss");
	const float wardAfterHit = shielded.ward();
	InputHandler::InputState idleInput;
	AdvancePlayer(shielded, idleInput, 3.2f);
	checks.expect(shielded.ward() > wardAfterHit, "ward regenerates after its cooldown");

	Player normalDamage({0.0f, 0.0f, 0.0f});
	Player vulnerableDamage({0.0f, 0.0f, 0.0f});
	const float normalLoss = normalDamage.takeIncomingDamage(10.0f, false);
	const float vulnerableLoss = vulnerableDamage.takeIncomingDamage(10.0f, true);
	checks.expect(vulnerableLoss > normalLoss, "open-coat vulnerability increases incoming damage");

	Player inventoryPlayer({0.0f, 0.0f, 0.0f});
	inventoryPlayer.collect(ReagentType::BoneSplinters, 2);
	CurseVector doubleBone;
	doubleBone.selectBase(BaseVectorType::Bottle);
	checks.expect(doubleBone.addReagent(ReagentType::BoneSplinters), "inventory spell selects the first duplicate reagent");
	checks.expect(doubleBone.addReagent(ReagentType::BoneSplinters), "inventory spell selects the second duplicate reagent");
	checks.expect(inventoryPlayer.consumeFor(doubleBone), "casting consumes selected stack quantities");
	checks.expect(inventoryPlayer.reagentQuantity(ReagentType::BoneSplinters) == 0,
		"duplicate reagent consumption decrements the full stack cost");

	Player coatPlayer({0.0f, 0.0f, 0.0f});
	coatPlayer.collect(ReagentType::BoneSplinters, 2);
	CurseVector coatSpell;
	CoatMenu coat;
	InputHandler::InputState coatInput;
	coatInput.rummageHeld = true;
	coatInput.baseSelection = 0;
	coatInput.reagentPressed[ReagentIndex(ReagentType::BoneSplinters)] = true;
	coat.update(coatInput, 0.016f, coatPlayer, coatSpell);
	checks.expect(coat.isActive() && coatSpell.base() == BaseVectorType::Bottle && coatSpell.reagents().size() == 1,
		"live coat input selects a base and reagent");
	coatInput.baseSelection = -1;
	coat.update(coatInput, 0.016f, coatPlayer, coatSpell);
	coat.update(coatInput, 0.016f, coatPlayer, coatSpell);
	checks.expect(coatSpell.reagents().size() == 2, "coat selection cannot reserve more copies than are carried");
	coatInput.rummageHeld = false;
	coatInput.reagentPressed.fill(false);
	coat.update(coatInput, 0.016f, coatPlayer, coatSpell);
	checks.expect(!coat.isActive(), "releasing the rummage control closes the coat");

	Enemy afflicted({0.0f, 0.0f, -2.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::Rotcap);
	SpellStats affliction;
	affliction.damage = 10.0f;
	affliction.poisonDps = 5.0f;
	affliction.snareStrength = 0.4f;
	affliction.fearDuration = 1.0f;
	affliction.duration = 2.0f;
	const float afflictedBefore = afflicted.health();
	afflicted.applyHex(affliction);
	const float afterImpact = afflicted.health();
	afflicted.updateCombat(0.5f, {0.0f, 0.0f, 0.0f}, {});
	checks.expect(afterImpact < afflictedBefore && afflicted.health() < afterImpact,
		"hex impact and rot both damage enemies");
	checks.expect(afflicted.position().z < -2.0f, "panic makes an afflicted enemy retreat");

	Enemy normalPursuer({0.0f, 0.0f, -5.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::BoneSplinters);
	Enemy snaredPursuer({0.0f, 0.0f, -5.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::BoneSplinters);
	SpellStats snareOnly;
	snareOnly.snareStrength = 0.5f;
	snareOnly.duration = 2.0f;
	snaredPursuer.applyHex(snareOnly);
	normalPursuer.updateCombat(0.5f, {0.0f, 0.0f, 0.0f}, {});
	snaredPursuer.updateCombat(0.5f, {0.0f, 0.0f, 0.0f}, {});
	checks.expect(normalPursuer.position().z > snaredPursuer.position().z, "thorn snare slows enemy pursuit");
	Enemy occludedHunter({0.0f, 0.0f, -4.0f}, EnemyArchetype::WitchHunter, 0, ReagentType::GraveSalt);
	const std::vector<BoundingBox> sightBlocker {{{-2.0f, 0.0f, -2.5f}, {2.0f, 3.0f, -1.5f}}};
	float occludedDamage = 0.0f;
	for (int i = 0; i < 40; ++i) {
		occludedDamage += occludedHunter.updateCombat(0.05f, {0.0f, 0.0f, 0.0f}, sightBlocker);
	}
	checks.expectNear(occludedDamage, 0.0f, 0.001f, "ranged hunters cannot attack through solid scenery");

	Pickup pickup({1.0f, 0.0f, 1.0f}, ReagentType::SaintAsh, 2);
	checks.expect(pickup.collectIfNear({1.0f, 0.0f, 1.0f}, 1.0f) && !pickup.isActive(),
		"world reagent pickups are collectable and deactivate");

	SpellStats seekingPayload;
	seekingPayload.homingStrength = 5.0f;
	ProjectileEffect seekingProjectile({0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -10.0f}, seekingPayload);
	seekingProjectile.updateProjectile(0.1f, {10.0f, 1.0f, -1.0f}, true);
	checks.expect(seekingProjectile.velocity().x > 0.0f && seekingProjectile.position().z < 0.0f,
		"Moth Dust homing turns a moving Bottle projectile toward an enemy");
	TrapEffect expiringTrap({0.0f, 0.0f, 0.0f}, spiderBase.stats());
	expiringTrap.update(18.1f);
	checks.expect(expiringTrap.expired(), "untriggered Spider traps expire cleanly");
	HexFieldEffect followingField({0.0f, 0.0f, 0.0f}, appleBase.stats(), 2.0f, true);
	checks.expect(followingField.takePulse(), "Apple aura begins with an immediate pulse");
	followingField.updateField(0.1f, {3.0f, 0.0f, 4.0f});
	checks.expectNear(followingField.position().x, 3.0f, 0.001f, "Apple aura follows the player");

	Game poppetGame;
	poppetGame.startNewRun();
	poppetGame.enemies_.clear();
	poppetGame.enemies_.emplace_back(Vector3 {0.0f, 0.0f, 12.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::BoneSplinters);
	const float poppetTargetHealth = poppetGame.enemies_.front().health();
	poppetGame.castSpell();
	checks.expect(poppetGame.enemies_.front().health() < poppetTargetHealth && !poppetGame.traces_.empty(),
		"centered Poppet ray hits a visible enemy body and creates a trace");
	Game blockedPoppetGame;
	blockedPoppetGame.startNewRun();
	blockedPoppetGame.enemies_.clear();
	blockedPoppetGame.enemies_.emplace_back(Vector3 {0.0f, 0.0f, 12.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::BoneSplinters);
	blockedPoppetGame.scenery_.emplace_back(BoundingBox {{-1.0f, 0.0f, 14.8f}, {1.0f, 2.5f, 15.2f}},
		SceneryStyle::Crate, true);
	const float blockedPoppetHealth = blockedPoppetGame.enemies_.front().health();
	blockedPoppetGame.castSpell();
	checks.expectNear(blockedPoppetGame.enemies_.front().health(), blockedPoppetHealth, 0.001f,
		"Poppet line-of-sight attacks stop at solid scenery");

	Game bottleGame;
	bottleGame.startNewRun();
	bottleGame.enemies_.clear();
	bottleGame.enemies_.emplace_back(Vector3 {0.0f, 0.0f, 12.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::GraveSalt);
	bottleGame.player_.collect(ReagentType::BoneSplinters);
	bottleGame.spell_.selectBase(BaseVectorType::Bottle);
	checks.expect(bottleGame.spell_.addReagent(ReagentType::BoneSplinters), "Bottle integration test selects its reagent");
	const float bottleTargetHealth = bottleGame.enemies_.front().health();
	bottleGame.castSpell();
	checks.expect(bottleGame.projectiles_.size() == 1 && bottleGame.player_.reagentQuantity(ReagentType::BoneSplinters) == 0,
		"Bottle cast launches a projectile and consumes its reagent");
	for (int i = 0; i < 800 && !bottleGame.projectiles_.empty(); ++i) {
		bottleGame.updateEffects(0.005f);
	}
	checks.expect(bottleGame.enemies_.front().health() < bottleTargetHealth && !bottleGame.bursts_.empty(),
		"Bottle projectile collides, applies its payload, and bursts");
	Game blockedBottleGame;
	blockedBottleGame.startNewRun();
	blockedBottleGame.enemies_.clear();
	blockedBottleGame.enemies_.emplace_back(Vector3 {0.0f, 0.0f, 12.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::GraveSalt);
	blockedBottleGame.scenery_.emplace_back(BoundingBox {{-1.0f, 0.0f, 14.9f}, {1.0f, 2.5f, 15.1f}},
		SceneryStyle::Crate, true);
	blockedBottleGame.spell_.selectBase(BaseVectorType::Bottle);
	const float blockedBottleHealth = blockedBottleGame.enemies_.front().health();
	blockedBottleGame.castSpell();
	blockedBottleGame.updateEffects(0.2f);
	checks.expect(blockedBottleGame.projectiles_.empty() && !blockedBottleGame.bursts_.empty(),
		"swept Bottle collision catches thin scenery between frames");
	checks.expectNear(blockedBottleGame.enemies_.front().health(), blockedBottleHealth, 0.001f,
		"scenery blocks a Bottle payload from enemies behind it");

	Game appleGame;
	appleGame.startNewRun();
	appleGame.enemies_.clear();
	appleGame.enemies_.emplace_back(Vector3 {0.0f, 0.0f, 17.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::MothDust);
	const float appleDamage = appleGame.player_.takeIncomingDamage(30.0f, false);
	(void)appleDamage;
	appleGame.player_.addWard(-80.0f);
	appleGame.player_.collect(ReagentType::BoneSplinters);
	appleGame.player_.collect(ReagentType::SaintAsh);
	appleGame.spell_.selectBase(BaseVectorType::Apple);
	checks.expect(appleGame.spell_.addReagent(ReagentType::BoneSplinters), "Apple integration test selects Bone Splinters");
	checks.expect(appleGame.spell_.addReagent(ReagentType::SaintAsh), "Apple integration test selects Saint Ash");
	const float appleHealthBefore = appleGame.player_.health();
	const float appleWardBefore = appleGame.player_.ward();
	const float auraTargetHealth = appleGame.enemies_.front().health();
	appleGame.castSpell();
	checks.expect(appleGame.player_.health() > appleHealthBefore && appleGame.player_.ward() > appleWardBefore,
		"Apple cast heals the player and applies Saint Ash ward gain");
	checks.expect(!appleGame.fields_.empty() && appleGame.fields_.front().followsPlayer(),
		"Apple cast creates a self-centered moving aura");
	checks.expect(appleGame.enemies_.front().health() < auraTargetHealth,
		"an offensive Apple reagent affects enemies within the aura");

	Game spiderGame;
	spiderGame.startNewRun();
	spiderGame.enemies_.clear();
	spiderGame.enemies_.emplace_back(Vector3 {0.0f, 0.0f, 5.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::SaintAsh);
	spiderGame.player_.collect(ReagentType::BoneSplinters);
	spiderGame.spell_.selectBase(BaseVectorType::Spider);
	checks.expect(spiderGame.spell_.addReagent(ReagentType::BoneSplinters), "Spider integration test selects its reagent");
	const float spiderTargetHealth = spiderGame.enemies_.front().health();
	spiderGame.castSpell();
	checks.expect(spiderGame.traps_.size() == 1, "Spider cast places a persistent trap");
	spiderGame.enemies_.front().setPosition(spiderGame.traps_.front().position());
	spiderGame.updateEffects(0.01f);
	checks.expect(spiderGame.traps_.empty() && spiderGame.enemies_.front().health() < spiderTargetHealth,
		"Spider trap detects an enemy and delivers its payload");

	Game mireGame;
	mireGame.startNewRun();
	mireGame.enemies_.clear();
	SpellStats mireStats;
	mireStats.damage = 4.0f;
	mireStats.radius = 2.0f;
	mireStats.poisonDps = 5.0f;
	mireStats.duration = 2.0f;
	mireStats.lingering = true;
	mireGame.detonate({0.0f, 0.0f, 0.0f}, mireStats);
	checks.expect(mireGame.fields_.size() == 1, "Rotcap delivery leaves a lingering hazard field");

	Game ashGame;
	ashGame.startNewRun();
	ashGame.enemies_.clear();
	ashGame.enemies_.emplace_back(Vector3 {0.0f, 0.0f, 0.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::SaintAsh);
	ashGame.player_.addWard(-80.0f);
	SpellStats ashStats;
	ashStats.damage = 1.0f;
	ashStats.radius = 1.0f;
	ashStats.wardGain = 18.0f;
	const float ashWardBefore = ashGame.player_.ward();
	ashGame.detonate({0.0f, 0.0f, 0.0f}, ashStats);
	checks.expect(ashGame.player_.ward() > ashWardBefore, "Saint Ash returns ward when a delivered hex affects an enemy");

	Game pickupGame;
	pickupGame.startNewRun();
	const ReagentType worldPickupType = pickupGame.pickups_.front().type();
	const Vector3 worldPickupPosition = pickupGame.pickups_.front().position();
	pickupGame.player_.setPosition({worldPickupPosition.x, 0.0f, worldPickupPosition.z});
	pickupGame.updatePickups(0.016f);
	checks.expect(pickupGame.player_.reagentQuantity(worldPickupType) == 1 && !pickupGame.pickups_.front().isActive(),
		"the player collects reagents from the world into stack inventory");

	Game dropGame;
	dropGame.startNewRun();
	dropGame.enemies_.clear();
	dropGame.enemies_.emplace_back(Vector3 {2.0f, 0.0f, 2.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::Rotcap);
	const std::size_t pickupCountBeforeDrop = dropGame.pickups_.size();
	SpellStats lethal;
	lethal.damage = 10000.0f;
	dropGame.enemies_.front().applyHex(lethal);
	dropGame.collectEnemyDrops();
	dropGame.collectEnemyDrops();
	checks.expect(dropGame.pickups_.size() == pickupCountBeforeDrop + 1 && dropGame.kills_ == 1,
		"dead enemies drop exactly one stackable reagent pickup");

	Game attackGame;
	attackGame.startNewRun();
	attackGame.enemies_.clear();
	attackGame.enemies_.emplace_back(Vector3 {0.0f, 0.0f, 17.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::BoneSplinters);
	for (int i = 0; i < 30; ++i) {
		attackGame.player_.updatePlayer(idleInput, 0.05f, {}, false);
		attackGame.updateEnemies(0.05f, false);
	}
	checks.expect(attackGame.player_.health() < attackGame.player_.maxHealth(), "enemy attacks damage an unwarded player");

	Game wardGame;
	wardGame.startNewRun();
	wardGame.enemies_.clear();
	wardGame.enemies_.emplace_back(Vector3 {0.0f, 0.0f, 17.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::BoneSplinters);
	for (int i = 0; i < 30; ++i) {
		wardGame.player_.updatePlayer(wardInput, 0.05f, {}, false);
		wardGame.updateEnemies(0.05f, false);
	}
	checks.expectNear(wardGame.player_.health(), wardGame.player_.maxHealth(), 0.001f,
		"ward integration absorbs live enemy attacks");
	checks.expect(wardGame.player_.ward() < wardGame.player_.maxWard(), "live ward defense spends ward resource");

	Game liveCoatGame;
	liveCoatGame.startNewRun();
	liveCoatGame.enemies_.clear();
	liveCoatGame.enemies_.emplace_back(Vector3 {0.0f, 0.0f, 17.0f}, EnemyArchetype::TorchBearer, 0, ReagentType::BoneSplinters);
	InputHandler::InputState liveCoatInput;
	liveCoatInput.rummageHeld = true;
	liveCoatInput.moveInput.x = 1.0f;
	const Vector3 liveCoatStart = liveCoatGame.player_.position();
	for (int i = 0; i < 30; ++i) {
		liveCoatGame.updatePlaying(liveCoatInput, 0.05f);
	}
	checks.expect(liveCoatGame.coatMenu_.isActive() && Vector3Distance(liveCoatStart, liveCoatGame.player_.position()) > 1.0f,
		"the coat remains open while the player keeps moving");
	checks.expect(liveCoatGame.player_.health() < liveCoatGame.player_.maxHealth(),
		"enemy combat continues while the player rummages");

	Game runGame;
	runGame.startNewRun();
	checks.expect(runGame.state_ == State::Playing && runGame.wave_ == 1 && runGame.livingEnemyCount() == 5,
		"a new run begins with the first five-hunter breach");
	const auto killLivingEnemies = [&lethal](Game& game) {
		for (Enemy& enemy : game.enemies_) {
			if (enemy.isAlive()) {
				enemy.applyHex(lethal);
			}
		}
	};
	const auto advanceRun = [&idleInput](Game& game, int frames) {
		for (int i = 0; i < frames; ++i) {
			game.updatePlaying(idleInput, 0.05f);
		}
	};
	killLivingEnemies(runGame);
	advanceRun(runGame, 60);
	checks.expect(runGame.wave_ == 2 && runGame.livingEnemyCount() == 7, "clearing breach one advances to seven hunters");
	killLivingEnemies(runGame);
	advanceRun(runGame, 60);
	checks.expect(runGame.wave_ == 3 && runGame.livingEnemyCount() == 9, "clearing breach two advances to nine hunters");
	killLivingEnemies(runGame);
	advanceRun(runGame, 50);
	checks.expect(runGame.state_ == State::Victory && runGame.kills_ == 21,
		"clearing all three breaches reaches victory with every kill counted");

	Game deathGame;
	deathGame.startNewRun();
	const float fatalDamage = deathGame.player_.takeDamage(1000.0f);
	(void)fatalDamage;
	deathGame.updatePlaying(idleInput, 0.016f);
	checks.expect(deathGame.state_ == State::GameOver, "lethal enemy damage reaches the restartable game-over state");

	report = checks.report();
	return checks.passed();
}
