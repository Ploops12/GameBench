#include "Game.h"
#include <raylib.h>

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

constexpr int screenWidth = 960;
constexpr int screenHeight = 720;

Game* game = nullptr;

void Run() {
	float dt = GetFrameTime();
	game->update(dt);
	game->draw();
}

int main() {
	constexpr int FPS_TARGET = 60;
	InitWindow(screenWidth, screenHeight, "Salem Hexshooter - Combat Slice");

	Game gameInstance(screenWidth, screenHeight);
	game = &gameInstance;

#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(Run, FPS_TARGET, 1);
#else
	SetTargetFPS(FPS_TARGET);

	while (!WindowShouldClose()) {
		Run();
	}
#endif

	CloseWindow();
	return 0;
}
