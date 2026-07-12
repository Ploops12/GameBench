#include <raylib.h>

#include "Game.h"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

namespace {
Game* runningGame = nullptr;

void runFrame() {
	if (runningGame != nullptr) {
		runningGame->update();
		runningGame->draw();
	}
}
}

int main() {
	constexpr int SCREEN_WIDTH = 1280;
	constexpr int SCREEN_HEIGHT = 720;
	constexpr int FPS_TARGET = 120;
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hexescape: The Salem Road");
	if (!IsWindowReady()) {
		return 1;
	}
	SetWindowMinSize(960, 600);

#if defined(PLATFORM_WEB)
	static Game game;
	runningGame = &game;
	emscripten_set_main_loop(runFrame, FPS_TARGET, 1);
#else
	{
		Game game;
		runningGame = &game;
		SetTargetFPS(FPS_TARGET);
		while (!WindowShouldClose()) {
			runFrame();
		}
		runningGame = nullptr;
	}
	CloseWindow();
#endif
	return 0;
}
