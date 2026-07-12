#pragma once

#include <array>
#include <raylib.h>

#include "Reagent.hxx"

struct GameAssets {
	GameAssets() = default;
	~GameAssets();
	GameAssets(const GameAssets&) = delete;
	GameAssets& operator=(const GameAssets&) = delete;
	GameAssets(GameAssets&&) = delete;
	GameAssets& operator=(GameAssets&&) = delete;

	std::array<Texture2D, REAGENT_TYPE_COUNT> reagentIcons {};
	std::array<Texture2D, 3> vectorIcons {};
	Texture2D poppetIcon {};
	Texture2D torchBearerSheet {};
	Texture2D witchHunterSheet {};
	Texture2D handsSheet {};
	bool loaded {false};

	bool load();
	void unload();
};
