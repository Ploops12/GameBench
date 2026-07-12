#include "Sprite.hxx"

Sprite::Sprite() : texture {}, source {0.0f, 0.0f, 0.0f, 0.0f}, ready(false) {}

Sprite::Sprite(Texture2D textureHandle, Rectangle sourceRect) : texture(textureHandle), source(sourceRect), ready(true) {}

bool Sprite::isReady() const {
	return ready;
}

void Sprite::setTexture(Texture2D textureHandle, Rectangle sourceRect) {
	texture = textureHandle;
	source = sourceRect;
	ready = true;
}

void Sprite::draw(Vector2 position, float scale, Color tint) const {
	if (!ready) {
		return;
	}

	Rectangle destination {
		position.x,
		position.y,
		source.width * scale,
		source.height * scale
	};
	DrawTexturePro(texture, source, destination, {0.0f, 0.0f}, 0.0f, tint);
}

Rectangle Sprite::getSourceRect() const {
	return source;
}
