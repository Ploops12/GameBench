#include "Sprite.hxx"

#include <limits>
#include <utility>

Sprite::Sprite(const std::string& filePath, int columns, int rows) {
	load(filePath, columns, rows);
}

Sprite::~Sprite() noexcept {
	unload();
}

Sprite::Sprite(Sprite&& other) noexcept {
	takeFrom(std::move(other));
}

Sprite& Sprite::operator=(Sprite&& other) noexcept {
	if (this != &other) {
		unload();
		takeFrom(std::move(other));
	}

	return *this;
}

bool Sprite::load(const std::string& filePath, int columns, int rows) {
	if (filePath.empty() || columns <= 0 || rows <= 0) return false;
	if (columns > std::numeric_limits<int>::max()/rows) return false;
	if (!IsWindowReady()) return false;

	Texture2D loadedTexture = LoadTexture(filePath.c_str());
	if (!IsTextureValid(loadedTexture)) {
		if (loadedTexture.id != 0 && IsWindowReady()) UnloadTexture(loadedTexture);
		return false;
	}

	if (columns > loadedTexture.width || rows > loadedTexture.height) {
		if (IsWindowReady()) UnloadTexture(loadedTexture);
		return false;
	}

	unload();
	texture_ = loadedTexture;
	path_ = filePath;
	columns_ = columns;
	rows_ = rows;
	loaded_ = true;
	return true;
}

void Sprite::unload() noexcept {
	if (loaded_ && texture_.id != 0 && IsWindowReady()) UnloadTexture(texture_);

	texture_ = {};
	path_.clear();
	columns_ = 1;
	rows_ = 1;
	loaded_ = false;
}

bool Sprite::loaded() const noexcept {
	return loaded_ && IsWindowReady() && IsTextureValid(texture_);
}

const std::string& Sprite::path() const noexcept {
	return path_;
}

const Texture2D& Sprite::texture() const noexcept {
	return texture_;
}

int Sprite::width() const noexcept {
	return loaded() ? texture_.width : 0;
}

int Sprite::height() const noexcept {
	return loaded() ? texture_.height : 0;
}

int Sprite::frameWidth() const noexcept {
	return loaded() ? texture_.width/columns_ : 0;
}

int Sprite::frameHeight() const noexcept {
	return loaded() ? texture_.height/rows_ : 0;
}

int Sprite::columns() const noexcept {
	return columns_;
}

int Sprite::rows() const noexcept {
	return rows_;
}

int Sprite::frameCount() const noexcept {
	return loaded() ? columns_*rows_ : 0;
}

bool Sprite::isFrameValid(int frameIndex) const noexcept {
	return frameIndex >= 0 && frameIndex < frameCount();
}

Rectangle Sprite::sourceRect(int frameIndex) const noexcept {
	if (!isFrameValid(frameIndex)) return {};

	const int frameWidth = this->frameWidth();
	const int frameHeight = this->frameHeight();
	const int column = frameIndex % columns_;
	const int row = frameIndex/columns_;

	return {
		static_cast<float>(column*frameWidth),
		static_cast<float>(row*frameHeight),
		static_cast<float>(frameWidth),
		static_cast<float>(frameHeight),
	};
}

void Sprite::draw(Vector2 position, Color tint) const {
	drawFrame(0, position, tint);
}

void Sprite::drawFrame(int frameIndex, Vector2 position, Color tint) const {
	const Rectangle source = sourceRect(frameIndex);
	if (source.width <= 0.0f || source.height <= 0.0f) return;

	DrawTextureRec(texture_, source, position, tint);
}

void Sprite::drawFramePro(int frameIndex, Rectangle destination, Vector2 origin,
	float rotation, Color tint) const {
	const Rectangle source = sourceRect(frameIndex);
	if (source.width <= 0.0f || source.height <= 0.0f) return;
	if (destination.width == 0.0f || destination.height == 0.0f) return;

	DrawTexturePro(texture_, source, destination, origin, rotation, tint);
}

void Sprite::drawBillboard(const Camera3D& camera, Vector3 position,
	float height, Color tint) const {
	drawBillboardFrame(camera, 0, position, height, tint);
}

void Sprite::drawBillboardFrame(const Camera3D& camera, int frameIndex,
	Vector3 position, float height, Color tint) const {
	const Rectangle source = sourceRect(frameIndex);
	if (source.width <= 0.0f || source.height <= 0.0f || height <= 0.0f) return;

	const Vector2 size {
		height*(source.width/source.height),
		height,
	};
	DrawBillboardRec(camera, texture_, source, position, size, tint);
}

void Sprite::drawBillboardFramePro(const Camera3D& camera, int frameIndex,
	Vector3 position, Vector3 up, Vector2 size, Vector2 origin,
	float rotation, Color tint) const {
	const Rectangle source = sourceRect(frameIndex);
	if (source.width <= 0.0f || source.height <= 0.0f) return;
	if (size.x == 0.0f || size.y == 0.0f) return;

	DrawBillboardPro(camera, texture_, source, position, up, size, origin, rotation, tint);
}

void Sprite::takeFrom(Sprite&& other) noexcept {
	texture_ = other.texture_;
	path_ = std::move(other.path_);
	columns_ = other.columns_;
	rows_ = other.rows_;
	loaded_ = other.loaded_;

	other.texture_ = {};
	other.columns_ = 1;
	other.rows_ = 1;
	other.loaded_ = false;
}
