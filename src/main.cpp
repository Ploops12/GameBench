#include <raylib.h>

#include "Game.h"

#include <iostream>
#include <string>

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

namespace {
	constexpr int SCREEN_WIDTH = 1280;
	constexpr int SCREEN_HEIGHT = 720;
	Game* gameInstance = nullptr;
}

bool RunSelfTest() {
	CurseVector bottle(BaseVector::Bottle);
	if (!bottle.addReagent(ReagentType::GraveSalt) || !bottle.addReagent(ReagentType::GraveSalt)) return false;
	if (!bottle.addReagent(ReagentType::WidowNettle) || bottle.addReagent(ReagentType::MothDust)) return false;
	const SpellStats bottleStats = bottle.stats();
	if (bottle.reagentCount() != 3 || bottle.countOf(ReagentType::GraveSalt) != 2 || bottleStats.damage != 32 || bottleStats.snareDuration <= 0.0f) return false;

	CurseVector apple(BaseVector::Apple);
	apple.addReagent(ReagentType::BlackWax);
	apple.addReagent(ReagentType::MothDust);
	if (apple.stats().poisonPerSecond <= 0.0f || !apple.stats().seeking || apple.outcomeText().find("rot->regen") == std::string::npos) return false;

	Player player;
	player.addReagent(ReagentType::WidowNettle, 2);
	if (player.reagentQuantity(ReagentType::WidowNettle) != 2 || !player.consumeReagent(ReagentType::WidowNettle, 1) || player.reagentQuantity(ReagentType::WidowNettle) != 1) return false;
	CoatMenu coat;
	player.addReagent(ReagentType::GraveSalt, 2);
	if (!coat.selectReagent(ReagentType::GraveSalt, player) || !coat.selectReagent(ReagentType::GraveSalt, player) || !coat.selectReagent(ReagentType::WidowNettle, player) || coat.selectReagent(ReagentType::MothDust, player) || coat.spell().reagentCount() != 3) return false;
	const float wardBefore = player.ward();
	InputHandler::InputState wardInput{};
	wardInput.wardHeld = true;
	player.update(0.1f, wardInput, {});
	if (!player.wardActive()) return false;
	const float healthBeforeWardHit = player.health();
	player.takeDamage(10.0f);
	if (player.health() != healthBeforeWardHit || player.ward() >= wardBefore) return false;
	wardInput.wardHeld = false;
	player.update(0.1f, wardInput, {});
	if (player.wardActive() || player.wardCooldown() <= 0.0f) return false;
	const float startingZ = player.getPosition().z;
	InputHandler::InputState movement{};
	movement.moveInput.x = 1.0f;
	player.update(0.1f, movement, {});
	if (player.getPosition().z >= startingZ) return false;

	std::cout << "self-test passed: vectors, duplicate reagents, inventory, and movement\n";
	return true;
}

void RunGame() {
	if (gameInstance == nullptr) return;
	gameInstance->update(GetFrameTime());
	gameInstance->draw();
}

#if defined(PLATFORM_WEB)
void RunGameWeb(void*) {
	RunGame();
}
#endif

int main(int argc, char** argv) {
	if (argc > 1 && std::string(argv[1]) == "--self-test") return RunSelfTest() ? 0 : 1;
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Salem Hexshooter");
	if (!IsWindowReady()) return 1;
	SetTargetFPS(60);

	Game game(SCREEN_WIDTH, SCREEN_HEIGHT);
	gameInstance = &game;

#if defined(PLATFORM_WEB)
	emcripten_set_main_loop_arg(RunGameWeb, nullptr, 60, 1);
#else
	while (!WindowShouldClose()) RunGame();
#endif

	CloseWindow();
	return 0;
}
