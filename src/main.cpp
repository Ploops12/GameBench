#include <raylib.h>
#include "Game.h"

#include <cmath>
#include <cstdio>
#include <cstring>

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;

Game* game = nullptr;
int framesRemaining = -1;

void Run() {
	game->update();
	game->draw();
	if (framesRemaining > 0) --framesRemaining;
}

int main(int argc, char** argv) {
	if (argc > 1 && std::strcmp(argv[1], "--self-test") == 0) {
		CurseVector poppet;
		if (poppet.type != VectorType::Poppet || poppet.reagentSlots != 0 || std::fabs(poppet.stats().damage - 12.0f) > 0.01f) return 1;
		CurseVector stacked;
		stacked.type = VectorType::Bottle;
		stacked.reagents = {ReagentType::GraveSalt, ReagentType::GraveSalt, ReagentType::WidowNettle};
		stacked.reagentSlots = 3;
		const SpellStats stats = stacked.stats();
		if (std::fabs(stats.damage - 38.0f) > 0.01f || std::fabs(stats.radius - 2.3f) > 0.01f || std::fabs(stats.snareSeconds - 1.6f) > 0.01f) return 2;
		std::puts("spell self-test passed");
		return 0;
	}
	constexpr int FPS_TARGET = 60;
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
	InitWindow(screenWidth, screenHeight, "Salem: Coat of Curses");
	if (argc > 1 && std::strcmp(argv[1], "--smoke-test") == 0) framesRemaining = 30;
	Game session;
	game = &session;

	{
#if defined(PLATFORM_WEB)
		emscripten_set_main_loop_arg(Run, FPS_TARGET, 1);
#else
		SetTargetFPS(FPS_TARGET);

		while (!WindowShouldClose() && framesRemaining != 0) {
			Run();
		}
#endif
	}

	CloseWindow();
	return 0;
}
