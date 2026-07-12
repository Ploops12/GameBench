#include "Sprite.hxx"

#include <array>
#include <utility>

namespace {

std::string resolveAssetPath(const std::string& fileName) {
	const std::array<std::string, 3> candidates {
		fileName,
		"../" + fileName,
		"../../" + fileName
	};

	for (const std::string& candidate : candidates) {
		if (FileExists(candidate.c_str())) {
			return candidate;
		}
	}

	return fileName;
}

}

Sprite::Sprite(const std::string& fileName) {
	load(fileName);
}

Sprite::~Sprite() {
	unload();
}

Sprite::Sprite(Sprite&& other) noexcept
	: texture(other.texture), loaded(other.loaded) {
	other.texture = {};
	other.loaded = false;
}

Sprite& Sprite::operator=(Sprite&& other) noexcept {
	if (this != &other) {
		unload();
		texture = other.texture;
		loaded = other.loaded;
		other.texture = {};
		other.loaded = false;
	}
	return *this;
}

bool Sprite::load(const std::string& fileName) {
	unload();
	const std::string resolvedPath = resolveAssetPath(fileName);
	if (!FileExists(resolvedPath.c_str())) {
		return false;
	}

	texture = LoadTexture(resolvedPath.c_str());
	loaded = texture.id != 0U;
	if (!loaded) {
		texture = {};
	}
	return loaded;
}

void Sprite::unload() {
	if (loaded) {
		UnloadTexture(texture);
	}
	texture = {};
	loaded = false;
}

bool Sprite::isReady() const {
	return loaded;
}

Texture2D Sprite::getTexture() const {
	return texture;
}

int Sprite::getWidth() const {
	return texture.width;
}

int Sprite::getHeight() const {
	return texture.height;
}

void Sprite::draw(Rectangle destination, Color tint) const {
	if (!loaded) {
		return;
	}
	drawFrame({0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height)}, destination, tint);
}

void Sprite::drawFrame(Rectangle source, Rectangle destination, Color tint) const {
	if (!loaded) {
		return;
	}
	DrawTexturePro(texture, source, destination, {0.0f, 0.0f}, 0.0f, tint);
}
