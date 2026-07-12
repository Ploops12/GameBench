#include "Animation.hxx"

#include <algorithm>

Animation::Animation(const Sprite* initialSprite, int initialFrameCount, float initialFramesPerSecond)
	: sprite(initialSprite) {
	configure(initialFrameCount, initialFramesPerSecond);
}

void Animation::setSprite(const Sprite* newSprite) {
	sprite = newSprite;
	reset();
}

void Animation::configure(int newFrameCount, float newFramesPerSecond) {
	frameCount = std::max(1, newFrameCount);
	framesPerSecond = std::max(0.01f, newFramesPerSecond);
	reset();
}

void Animation::update(float deltaTime) {
	if (frameCount <= 1) {
		return;
	}

	elapsed += deltaTime;
	const float frameDuration = 1.0f / framesPerSecond;
	while (elapsed >= frameDuration) {
		elapsed -= frameDuration;
		frame = (frame + 1) % frameCount;
	}
}

void Animation::reset() {
	frame = 0;
	elapsed = 0.0f;
}

int Animation::getFrame() const {
	return frame;
}

Rectangle Animation::getSourceRectangle() const {
	if (sprite == nullptr || !sprite->isReady()) {
		return {};
	}

	const float frameWidth = static_cast<float>(sprite->getWidth()) / static_cast<float>(frameCount);
	return {frameWidth * static_cast<float>(frame), 0.0f, frameWidth, static_cast<float>(sprite->getHeight())};
}

void Animation::draw(Rectangle destination, Color tint) const {
	if (sprite != nullptr && sprite->isReady()) {
		sprite->drawFrame(getSourceRectangle(), destination, tint);
	}
}
