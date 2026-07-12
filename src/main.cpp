#include <raylib.h>

#include "Game.h"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

namespace {
	Game* game = nullptr;

	void Run() {
		float dt = GetFrameTime();
		if (dt > 0.05f) dt = 0.05f;
		game->Update(dt);
		game->Draw();
	}
}

int main() {
	constexpr int screenWidth = 1280;
	constexpr int screenHeight = 720;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(screenWidth, screenHeight, "Salem Hexshooter // Cauldron");
	if (!IsWindowReady()) return 1;
	SetTargetFPS(60);
	DisableCursor();

	{
		Game runningGame;
		game = &runningGame;
#if defined(PLATFORM_WEB)
		emscripten_set_main_loop(Run, 60, 1);
#else
		while (!runningGame.ShouldExit()) Run();
#endif
		game = nullptr;
	}

	EnableCursor();
	CloseWindow();
	return 0;
}
