#include <raylib.h>
#include "InputHandler.hxx"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

constexpr int screenWidth = 720;
constexpr int screenHeight = 720;

InputHandler inputHandler;

void Update() {
	float dt = GetFrameTime();
	if (dt > 0.05f) dt = 0.05f;
	InputHandler::InputState input = inputHandler.poll();
}

void Draw() {
	BeginDrawing();
	EndDrawing();
}

void Run() {
	Update();
	Draw();
}

int main() {
	constexpr int FPS_TARGET = 60;
	InitWindow(screenWidth, screenHeight, "Cauldron");

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
