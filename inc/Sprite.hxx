#pragma once

#include <raylib.h>

class Sprite {
public:
	Sprite();
	Sprite(Texture2D textureHandle, Rectangle sourceRect);

	bool isReady() const;
	void setTexture(Texture2D textureHandle, Rectangle sourceRect);
	void draw(Vector2 position, float scale, Color tint) const;
	Rectangle getSourceRect() const;

private:
	Texture2D texture;
	Rectangle source;
	bool ready;
};
