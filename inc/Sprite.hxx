#pragma once

#include <raylib.h>

class Sprite {
public:
	bool load(const char *path) { texture = LoadTexture(path); return texture.id != 0; }
	void unload() { if (texture.id != 0) UnloadTexture(texture); texture = {}; }
	Texture2D getTexture() const { return texture; }

private:
	Texture2D texture{};
};
