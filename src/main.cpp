#include <raylib.h>
#include "Game.h"
#include "MainMenu.h"

#include <cstdio>
#include <cstring>
#include <string>

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;

Game* game = nullptr;
MainMenu* mainMenu = nullptr;

void Update() {
	float dt = GetFrameTime();
	if (dt > 0.05f) dt = 0.05f;
	if (mainMenu && mainMenu->isActive()) {
		mainMenu->update(dt);
		if (mainMenu->consumePlayRequested()) {
			DisableCursor();
		}
		return;
	}
	game->update(dt);
}

void Draw() {
	if (mainMenu && mainMenu->isActive()) {
		mainMenu->draw();
		return;
	}
	game->draw();
}

void Run() {
	Update();
	Draw();
}

int main(int argc, char** argv) {
	bool skipMenu = false;
	if (argc > 1 && std::strcmp(argv[1], "--self-test") == 0) {
		std::string error;
		if (!runVerticalSliceSelfTest(&error)) {
			std::fprintf(stderr, "self-test failed: %s\n", error.c_str());
			return 2;
		}
		std::printf("self-test passed\n");
		return 0;
	}
	if (argc > 1 && std::strcmp(argv[1], "--play") == 0) {
		skipMenu = true;
	}

	constexpr int FPS_TARGET = 60;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(screenWidth, screenHeight, "Salem Hexshooter - Combat Slice");
	if (!IsWindowReady()) {
		return 1;
	}
	Game localGame;
	MainMenu localMainMenu;
	game = &localGame;
	mainMenu = &localMainMenu;
	if (skipMenu) {
		mainMenu->setActive(false);
		DisableCursor();
	} else {
		EnableCursor();
	}

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
