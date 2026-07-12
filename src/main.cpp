#include <raylib.h>
#include "Game.h"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

constexpr int screenWidth = 720;
constexpr int screenHeight = 720;

Game game;

void Update() {
	float dt = GetFrameTime();
	if (dt > 0.05f) dt = 0.05f;
	game.update(dt);
}

void Draw() {
	BeginDrawing();
	game.draw();
	EndDrawing();
}

void Run() {
	Update();
	Draw();
}

int main() {
	constexpr int FPS_TARGET = 60;
	InitWindow(screenWidth, screenHeight, "Cauldron");
	if (!IsWindowReady()) return 1;
	DisableCursor();

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
