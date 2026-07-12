#include <raylib.h>
#include "Game.h"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;

Game* game = nullptr;

void Run() {
	game->update();
	game->draw();
}

int main() {
	constexpr int FPS_TARGET = 60;
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(screenWidth, screenHeight, "Salem: Satchel of Thorns");
	SetWindowMinSize(960, 640);
	Game activeGame(screenWidth, screenHeight);
	game = &activeGame;

	{
#if defined(PLATFORM_WEB)
		emscripten_set_main_loop(Run, FPS_TARGET, 1);
#else
		SetTargetFPS(FPS_TARGET);

		while (!WindowShouldClose()) {
			Run();
		}
#endif
	}

	game = nullptr;
	CloseWindow();
	return 0;
}
