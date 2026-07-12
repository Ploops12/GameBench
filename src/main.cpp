#include <raylib.h>

#include "Game.h"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

namespace {
constexpr int ScreenWidth = 1280;
constexpr int ScreenHeight = 720;

Game* activeGame = nullptr;

void runFrame() {
	activeGame->update();
	activeGame->draw();
}
}

int main() {
	constexpr int FramesPerSecond = 60;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(ScreenWidth, ScreenHeight, "Salem: Satchel of Thorns");
	if (!IsWindowReady()) {
		return 1;
	}

	{
		Game game;
		activeGame = &game;
#if defined(PLATFORM_WEB)
		emscripten_set_main_loop(runFrame, FramesPerSecond, 1);
#else
		SetTargetFPS(FramesPerSecond);
		while (!WindowShouldClose()) {
			runFrame();
		}
#endif
		activeGame = nullptr;
	}

	CloseWindow();
	return 0;
}
