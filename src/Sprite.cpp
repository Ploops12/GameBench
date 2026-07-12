#include <utility>
#include "Sprite.hxx"

Sprite::~Sprite() {
	unload();
}

Sprite::Sprite(Sprite&& other) noexcept
	: texture(other.texture), loaded(other.loaded) {
	other.texture = {};
	other.loaded = false;
}

Sprite& Sprite::operator=(Sprite&& other) noexcept {
	if (this == &other) {
		return *this;
	}

	unload();
	texture = other.texture;
	loaded = other.loaded;
	other.texture = {};
	other.loaded = false;
	return *this;
}

bool Sprite::loadFromFile(const std::string& path) {
	unload();
	texture = LoadTexture(path.c_str());
	loaded = texture.id != 0;
	return loaded;
}

void Sprite::unload() {
	if (loaded) {
		UnloadTexture(texture);
		texture = {};
		loaded = false;
	}
}

bool Sprite::isLoaded() const {
	return loaded;
}

const Texture2D& Sprite::getTexture() const {
	return texture;
}

Texture2D& Sprite::getTexture() {
	return texture;
}

Rectangle Sprite::frame(int columns, int rows, int index) const {
	if (!loaded || columns <= 0 || rows <= 0) {
		return Rectangle {0, 0, 0, 0};
	}

	const float frameWidth = static_cast<float>(texture.width) / static_cast<float>(columns);
	const float frameHeight = static_cast<float>(texture.height) / static_cast<float>(rows);
	const int wrappedIndex = index % (columns * rows);
	const int x = wrappedIndex % columns;
	const int y = wrappedIndex / columns;

	return Rectangle {
		frameWidth * static_cast<float>(x),
		frameHeight * static_cast<float>(y),
		frameWidth,
		frameHeight
	};
}
