#include "Sprite.hxx"

#include <algorithm>
#include <utility>

Sprite::Sprite(const std::string& path, int columns, int rows) {
	const bool loaded = load(path, columns, rows);
	(void)loaded;
}

Sprite::~Sprite() {
	unload();
}

Sprite::Sprite(Sprite&& other) noexcept
	: texture_(other.texture_), columns_(other.columns_), rows_(other.rows_), frame_(other.frame_) {
	other.texture_ = {};
}

Sprite& Sprite::operator=(Sprite&& other) noexcept {
	if (this == &other) {
		return *this;
	}
	unload();
	texture_ = other.texture_;
	columns_ = other.columns_;
	rows_ = other.rows_;
	frame_ = other.frame_;
	other.texture_ = {};
	return *this;
}

bool Sprite::load(const std::string& path, int columns, int rows) {
	unload();
	if (!FileExists(path.c_str())) {
		return false;
	}
	texture_ = LoadTexture(path.c_str());
	columns_ = std::max(1, columns);
	rows_ = std::max(1, rows);
	frame_ = 0;
	return texture_.id != 0;
}

void Sprite::unload() {
	if (texture_.id != 0 && IsWindowReady()) {
		UnloadTexture(texture_);
	}
	texture_ = {};
}

void Sprite::setFrame(int frame) {
	frame_ = std::clamp(frame, 0, frameCount() - 1);
}

int Sprite::frame() const {
	return frame_;
}

int Sprite::frameCount() const {
	return columns_ * rows_;
}

bool Sprite::isLoaded() const {
	return texture_.id != 0;
}

Rectangle Sprite::source() const {
	if (!isLoaded()) {
		return {};
	}
	const float width = static_cast<float>(texture_.width) / static_cast<float>(columns_);
	const float height = static_cast<float>(texture_.height) / static_cast<float>(rows_);
	return {
		static_cast<float>(frame_ % columns_) * width,
		static_cast<float>(frame_ / columns_) * height,
		width,
		height
	};
}

void Sprite::draw(Vector2 position, float scale, Color tint) const {
	if (!isLoaded()) {
		return;
	}
	const Rectangle src = source();
	DrawTexturePro(texture_, src, {position.x, position.y, src.width * scale, src.height * scale}, {}, 0.0f, tint);
}

void Sprite::drawPro(Rectangle destination, Vector2 origin, float rotation, Color tint) const {
	if (isLoaded()) {
		DrawTexturePro(texture_, source(), destination, origin, rotation, tint);
	}
}

void Sprite::drawBillboard(const Camera3D& camera, Vector3 position, float scale, Color tint) const {
	if (!isLoaded()) {
		return;
	}
	const Rectangle src = source();
	const float aspect = src.width / std::max(1.0f, src.height);
	DrawBillboardRec(camera, texture_, src, position, {scale * aspect, scale}, tint);
}
