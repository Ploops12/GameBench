#include "Sprite.hxx"

Sprite::~Sprite() {
	Unload();
}

bool Sprite::Load(const char* path) {
	Unload();
	texture = LoadTexture(path);
	return IsReady();
}

void Sprite::Unload() {
	if (IsReady()) {
		UnloadTexture(texture);
		texture = {};
	}
}

Rectangle Sprite::GetFrame(int columns, int rows, int index) const {
	if (!IsReady() || columns <= 0 || rows <= 0) return {};
	const int frameCount = columns * rows;
	index %= frameCount;
	if (index < 0) index += frameCount;
	const float width = static_cast<float>(texture.width) / columns;
	const float height = static_cast<float>(texture.height) / rows;
	return {static_cast<float>(index % columns) * width, static_cast<float>(index / columns) * height, width, height};
}
