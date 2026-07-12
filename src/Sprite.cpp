#include "Sprite.hxx"

#include <algorithm>
#include <utility>

Sprite::Sprite(const std::string& path, int columnCount, int rowCount) {
	load(path, columnCount, rowCount);
}

Sprite::~Sprite() {
	unload();
}

Sprite::Sprite(Sprite&& other) noexcept
	: texture(other.texture), columns(other.columns), rows(other.rows) {
	other.texture = {};
	other.columns = 1;
	other.rows = 1;
}

Sprite& Sprite::operator=(Sprite&& other) noexcept {
	if (this == &other) return *this;

	unload();
	texture = other.texture;
	columns = other.columns;
	rows = other.rows;
	other.texture = {};
	other.columns = 1;
	other.rows = 1;
	return *this;
}

bool Sprite::load(const std::string& path, int columnCount, int rowCount) {
	unload();
	columns = std::max(1, columnCount);
	rows = std::max(1, rowCount);

	if (!IsWindowReady() || path.empty() || !FileExists(path.c_str())) return false;

	texture = LoadTexture(path.c_str());
	if (!IsTextureValid(texture)) {
		texture = {};
		return false;
	}

	SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
	return true;
}

void Sprite::unload() {
	if (texture.id != 0 && IsWindowReady()) UnloadTexture(texture);
	texture = {};
}

bool Sprite::isLoaded() const {
	return IsTextureValid(texture);
}

Rectangle Sprite::getFrameSource(int frame) const {
	if (!isLoaded()) return {};

	const int frameCount = getFrameCount();
	frame %= frameCount;
	if (frame < 0) frame += frameCount;

	const float frameWidth = static_cast<float>(texture.width) / static_cast<float>(columns);
	const float frameHeight = static_cast<float>(texture.height) / static_cast<float>(rows);
	return {
		static_cast<float>(frame % columns) * frameWidth,
		static_cast<float>(frame / columns) * frameHeight,
		frameWidth,
		frameHeight
	};
}

void Sprite::draw2D(int frame, Rectangle destination, Color tint) const {
	if (!isLoaded()) return;
	DrawTexturePro(texture, getFrameSource(frame), destination, {}, 0.0f, tint);
}

void Sprite::drawBillboard(const Camera3D& camera, int frame, Vector3 position, Vector2 size,
	Color tint) const {
	if (!isLoaded() || size.x <= 0.0f || size.y <= 0.0f) return;
	DrawBillboardRec(camera, texture, getFrameSource(frame), position, size, tint);
}

const Texture2D& Sprite::getTexture() const {
	return texture;
}

int Sprite::getFrameCount() const {
	return columns * rows;
}
