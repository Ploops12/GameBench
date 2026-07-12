#pragma once

#include <raylib.h>

class Sprite {
public:
	Sprite() = default;
	~Sprite();
	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;

	bool Load(const char* path);
	void Unload();
	bool IsReady() const { return texture.id != 0; }
	Texture2D GetTexture() const { return texture; }
	Rectangle GetFrame(int columns, int rows, int index) const;

private:
	Texture2D texture{};
};
