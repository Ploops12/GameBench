#include "Animation.hxx"

#include <algorithm>
#include <utility>

Sprite::Sprite(const char* path, int frameCount) {
	load(path, frameCount);
}

Sprite::~Sprite() {
	unload();
}

Sprite::Sprite(Sprite&& other) noexcept {
	*this = std::move(other);
}

Sprite& Sprite::operator=(Sprite&& other) noexcept {
	if (this == &other) return *this;
	unload();
	image = other.image;
	frameTotal = other.frameTotal;
	loaded = other.loaded;
	other.image = {};
	other.frameTotal = 1;
	other.loaded = false;
	return *this;
}

bool Sprite::load(const char* path, int frameCount) {
	unload();
	image = LoadTexture(path);
	loaded = image.id != 0;
	frameTotal = std::max(1, frameCount);
	return loaded;
}

void Sprite::unload() {
	if (!loaded || image.id == 0) return;
	UnloadTexture(image);
	image = {};
	loaded = false;
	frameTotal = 1;
}

bool Sprite::isLoaded() const {
	return loaded;
}

int Sprite::frames() const {
	return frameTotal;
}

void Sprite::setFrameCountForTest(int frameCount) {
	frameTotal = std::max(1, frameCount);
}

Rectangle Sprite::frameRect(int frameIndex) const {
	if (!loaded || frameTotal <= 0) return {};
	const int clampedFrame = std::clamp(frameIndex, 0, frameTotal - 1);
	const float frameWidth = static_cast<float>(image.width) / static_cast<float>(frameTotal);
	return {frameWidth * clampedFrame, 0.0f, frameWidth, static_cast<float>(image.height)};
}

void Sprite::draw(Rectangle destination, int frameIndex, Color tint) const {
	if (!loaded) return;
	DrawTexturePro(image, frameRect(frameIndex), destination, {}, 0.0f, tint);
}

const Texture2D& Sprite::texture() const {
	return image;
}

Animation::Animation(const char* path, int frameCount, float secondsPerFrame) {
	load(path, frameCount, secondsPerFrame);
}

bool Animation::load(const char* path, int frameCount, float secondsPerFrame) {
	frameTime = std::max(0.001f, secondsPerFrame);
	elapsed = 0.0f;
	frame = 0;
	return sprite.load(path, frameCount);
}

void Animation::update(float dt) {
	const int frameCount = sprite.isLoaded() ? sprite.frames() : testFrameTotal;
	if (frameCount <= 1) return;
	elapsed += dt;
	while (elapsed >= frameTime) {
		elapsed -= frameTime;
		frame = (frame + 1) % frameCount;
	}
}

void Animation::reset() {
	elapsed = 0.0f;
	frame = 0;
}

void Animation::draw(Rectangle destination, Color tint) const {
	sprite.draw(destination, frame, tint);
}

bool Animation::isLoaded() const {
	return sprite.isLoaded() || testFrameTotal > 0;
}

int Animation::currentFrame() const {
	return frame;
}

void Animation::setFrameCountForTest(int frameCount) {
	testFrameTotal = std::max(1, frameCount);
	frame = 0;
	elapsed = 0.0f;
	frameTime = 0.1f;
}
