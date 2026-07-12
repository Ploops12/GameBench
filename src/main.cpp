#include <raylib.h>
#include "Game.h"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;

Game* game = nullptr;

void Update() {
	game->update();
}

void Draw() {
	game->draw();
}

void Run() {
	Update();
	Draw();
}

int main() {
	constexpr int FPS_TARGET = 60;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(screenWidth, screenHeight, "Cauldron - Salem Hexshooter");
	if (!IsWindowReady()) {
		return 1;
	}
	game = new Game(screenWidth, screenHeight);

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

	delete game;
	CloseWindow();
	return 0;
}
