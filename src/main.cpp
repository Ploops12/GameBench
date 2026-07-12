#include <raylib.h>
#include <string>
#include <memory>
#include "Game.h"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;

std::unique_ptr<Game> game;

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

int main(int argc, char** argv) {
	if (argc > 1 && std::string(argv[1]) == "--logic-smoke") {
		Game smokeGame;
		std::string report;
		bool ok = smokeGame.runLogicSmoke(report);
		TraceLog(LOG_INFO, "%s", report.c_str());
		return ok ? 0 : 2;
	}

	constexpr int FPS_TARGET = 60;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(screenWidth, screenHeight, "Salem Hexshooter");
	if (!IsWindowReady()) {
		return 1;
	}
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
