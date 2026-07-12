#include <raylib.h>
#include "Game.h"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;

Game* game = nullptr;

void Run() {
	float deltaTime = GetFrameTime();
	if (deltaTime > 0.05f) deltaTime = 0.05f;
	game->update(deltaTime);
	game->draw();
}

int main() {
	constexpr int FPS_TARGET = 60;
	InitWindow(screenWidth, screenHeight, "Salem Hexshooter // Coat of Thorns");
	if (!IsWindowReady()) return 1;
	SetWindowMinSize(960, 540);
	Game runtime;
	game = &runtime;

	{
#if defined(PLATFORM_WEB)
		emscripten_set_main_loop_arg(Run, FPS_TARGET, 1);
#else
		SetTargetFPS(FPS_TARGET);

		while (!WindowShouldClose()) {
			Run();
		}
#endif
	}

	CloseWindow();
	return 0;
}
