#include "Sprite.hxx"
#include <algorithm>
#include <utility>

Sprite::Sprite(const char* path, int columns, int rows) {
	load(path, columns, rows);
}

Sprite::~Sprite() {
	unload();
}

Sprite::Sprite(Sprite&& other) noexcept :
	texture(other.texture), columns(other.columns), rows(other.rows), frame(other.frame) {
	other.texture = {};
}

Sprite& Sprite::operator=(Sprite&& other) noexcept {
	if (this == &other) return *this;
	unload();
	texture = other.texture;
	columns = other.columns;
	rows = other.rows;
	frame = other.frame;
	other.texture = {};
	return *this;
}

bool Sprite::load(const char* path, int newColumns, int newRows) {
	unload();
	columns = std::max(1, newColumns);
	rows = std::max(1, newRows);
	frame = 0;
	texture = LoadTexture(path);
	return isReady();
}

void Sprite::unload() {
	if (isReady()) UnloadTexture(texture);
	texture = {};
}

bool Sprite::isReady() const {
	return texture.id != 0 && texture.width > 0 && texture.height > 0;
}

void Sprite::setFrame(int index) {
	frame = std::clamp(index, 0, getFrameCount() - 1);
}

Rectangle Sprite::getSource() const {
	if (!isReady()) return {};
	const float frameWidth = static_cast<float>(texture.width) / columns;
	const float frameHeight = static_cast<float>(texture.height) / rows;
	return Rectangle{
		static_cast<float>(frame % columns) * frameWidth,
		static_cast<float>(frame / columns) * frameHeight,
		frameWidth,
		frameHeight
	};
}

void Sprite::draw(Vector2 position, float scale, Color tint) const {
	if (!isReady()) return;
	const Rectangle source = getSource();
	const Rectangle destination{position.x, position.y, source.width * scale, source.height * scale};
	DrawTexturePro(texture, source, destination, Vector2{}, 0.0f, tint);
}

void Sprite::drawBillboard(const Camera3D& camera, Vector3 position, float size, Color tint) const {
	if (!isReady()) return;
	const Rectangle source = getSource();
	const float aspect = source.width / source.height;
	DrawBillboardRec(camera, texture, source, position, Vector2{size * aspect, size}, tint);
}
