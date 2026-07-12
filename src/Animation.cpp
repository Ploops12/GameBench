#include "Animation.hxx"

#include <algorithm>
#include <cmath>
#include <utility>

Animation::Animation(Sprite sprite, float framesPerSecond, bool looping)
	: sprite_(std::move(sprite)), looping_(looping) {
	setFramesPerSecond(framesPerSecond);
	playing_ = sprite_.loaded();
}

Animation::Animation(const std::string& filePath, int columns, int rows,
	float framesPerSecond, bool looping) {
	load(filePath, columns, rows, framesPerSecond, looping);
}

bool Animation::load(const std::string& filePath, int columns, int rows,
	float framesPerSecond, bool looping) {
	if (!sprite_.load(filePath, columns, rows)) return false;

	framesPerSecond_ = 0.0f;
	setFramesPerSecond(framesPerSecond);
	looping_ = looping;
	reset();
	playing_ = true;
	return true;
}

bool Animation::loaded() const noexcept {
	return sprite_.loaded();
}

Sprite& Animation::sprite() noexcept {
	return sprite_;
}

const Sprite& Animation::sprite() const noexcept {
	return sprite_;
}

void Animation::setFramesPerSecond(float framesPerSecond) noexcept {
	framesPerSecond_ = std::isfinite(framesPerSecond)
		? std::max(0.0f, framesPerSecond)
		: 0.0f;
	if (framesPerSecond_ == 0.0f) elapsedInFrame_ = 0.0f;
}

float Animation::framesPerSecond() const noexcept {
	return framesPerSecond_;
}

float Animation::frameDuration() const noexcept {
	return framesPerSecond_ > 0.0f ? 1.0f/framesPerSecond_ : 0.0f;
}

void Animation::setLooping(bool looping) noexcept {
	looping_ = looping;
	if (looping_) finished_ = false;
}

bool Animation::looping() const noexcept {
	return looping_;
}

bool Animation::playing() const noexcept {
	return playing_;
}

bool Animation::finished() const noexcept {
	return finished_;
}

int Animation::currentFrame() const noexcept {
	return currentFrame_;
}

void Animation::play() noexcept {
	if (!loaded()) return;

	if (finished_) reset();
	playing_ = true;
}

void Animation::pause() noexcept {
	playing_ = false;
}

void Animation::stop() noexcept {
	playing_ = false;
	reset();
}

void Animation::reset() noexcept {
	currentFrame_ = 0;
	elapsedInFrame_ = 0.0f;
	finished_ = false;
}

void Animation::setFrame(int frameIndex) noexcept {
	const int count = sprite_.frameCount();
	if (count == 0) {
		currentFrame_ = 0;
		elapsedInFrame_ = 0.0f;
		finished_ = false;
		return;
	}

	currentFrame_ = std::clamp(frameIndex, 0, count - 1);
	elapsedInFrame_ = 0.0f;
	finished_ = !looping_ && currentFrame_ == count - 1;
	if (finished_) playing_ = false;
}

void Animation::update(float deltaSeconds) noexcept {
	const int count = sprite_.frameCount();
	if (!playing_ || !loaded() || count <= 1 || framesPerSecond_ <= 0.0f) return;
	if (!std::isfinite(deltaSeconds) || deltaSeconds <= 0.0f) return;

	const double duration = static_cast<double>(frameDuration());
	const double elapsed = static_cast<double>(elapsedInFrame_) + deltaSeconds;
	if (elapsed < duration) {
		elapsedInFrame_ = static_cast<float>(elapsed);
		return;
	}

	const double advance = std::floor(elapsed/duration);
	elapsedInFrame_ = static_cast<float>(std::fmod(elapsed, duration));

	if (looping_) {
		const int wrappedAdvance = static_cast<int>(std::fmod(advance,
			static_cast<double>(count)));
		currentFrame_ = (currentFrame_ + wrappedAdvance)%count;
		return;
	}

	const int remainingFrames = count - 1 - currentFrame_;
	if (advance >= static_cast<double>(remainingFrames)) {
		currentFrame_ = count - 1;
		elapsedInFrame_ = 0.0f;
		playing_ = false;
		finished_ = true;
		return;
	}

	currentFrame_ += static_cast<int>(advance);
}

void Animation::draw(Vector2 position, Color tint) const {
	sprite_.drawFrame(currentFrame_, position, tint);
}

void Animation::drawPro(Rectangle destination, Vector2 origin, float rotation,
	Color tint) const {
	sprite_.drawFramePro(currentFrame_, destination, origin, rotation, tint);
}

void Animation::drawBillboard(const Camera3D& camera, Vector3 position,
	float height, Color tint) const {
	sprite_.drawBillboardFrame(camera, currentFrame_, position, height, tint);
}

void Animation::drawBillboardPro(const Camera3D& camera, Vector3 position,
	Vector3 up, Vector2 size, Vector2 origin, float rotation, Color tint) const {
	sprite_.drawBillboardFramePro(camera, currentFrame_, position, up, size,
		origin, rotation, tint);
}
