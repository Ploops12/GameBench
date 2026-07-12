#pragma once

#include <raylib.h>

class Sprite {
public:
	Sprite() = default;
	~Sprite();

	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;

	bool load(const char* fileName);
	void unload();
	bool isLoaded() const;

	void draw(Rectangle source, Rectangle destination, Color tint = WHITE) const;
	int width() const;
	int height() const;

private:
	Texture2D texture{};
};
