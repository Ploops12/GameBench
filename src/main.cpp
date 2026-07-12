#include <raylib.h>
#include <memory>
#include <string>
#include "Game.h"
#include "SelfTest.hxx"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;

std::unique_ptr<Game> game;

void Update() {
	float dt = GetFrameTime();
	if (dt > 0.05f) dt = 0.05f;
	game->update(dt);
}

void Draw() {
	BeginDrawing();
	game->draw();
	EndDrawing();
}

void Run() {
	Update();
	Draw();
}

int main(int argc, char** argv) {
	if (argc > 1 && std::string(argv[1]) == "--self-test") return runSelfTests() ? 0 : 1;
	constexpr int FPS_TARGET = 60;
	InitWindow(screenWidth, screenHeight, "Cauldron");
	SetWindowTitle("SATCHEL OF THORNS // SALEM COMBAT SLICE");
	game = std::make_unique<Game>();

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
