#include "Game.h"

#include <raylib.h>

#include <cstring>
#include <iostream>
#include <string>

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

namespace {
Game* activeGame = nullptr;

void RunFrame() {
	float dt = GetFrameTime();
	if (dt > 0.05f) dt = 0.05f;
	activeGame->update(dt);
	activeGame->draw();
}

int RunSelfTest() {
	std::string report;
	const bool passed = Game::runDeterministicChecks(report);
	std::cout << report << '\n';
	return passed ? 0 : 1;
}
}

int main(int argc, char** argv) {
	if (argc > 1 && std::strcmp(argv[1], "--self-test") == 0) {
		return RunSelfTest();
	}
	const bool runtimeSmoke = argc > 1 && std::strcmp(argv[1], "--runtime-smoke") == 0;

	constexpr int screenWidth = 1280;
	constexpr int screenHeight = 720;
	constexpr int fpsTarget = 60;
	unsigned int windowFlags = FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT;
	if (runtimeSmoke) {
		windowFlags |= FLAG_WINDOW_HIDDEN;
		SetTraceLogLevel(LOG_WARNING);
	}
	SetConfigFlags(windowFlags);
	InitWindow(screenWidth, screenHeight, "Thornwake: A Salem Hexshooter");
	InitAudioDevice();
	SetWindowMinSize(960, 680);
	SetTargetFPS(fpsTarget);

	{
		Game game;
		if (runtimeSmoke) {
			game.startNewRun();
			for (int frame = 0; frame < 120; ++frame) {
				game.update(1.0f / static_cast<float>(fpsTarget));
				game.draw();
			}
			std::cout << "runtime render smoke: PASS\n";
		} else {
			activeGame = &game;
#if defined(PLATFORM_WEB)
			emscripten_set_main_loop(RunFrame, fpsTarget, 1);
#else
			while (!game.shouldClose()) {
				RunFrame();
			}
#endif
			activeGame = nullptr;
		}
	}

	if (IsAudioDeviceReady()) {
		CloseAudioDevice();
	}
	CloseWindow();
	return 0;
}
