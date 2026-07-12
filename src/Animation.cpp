#include "Animation.hxx"

#include <algorithm>
#include <cmath>
#include <utility>

Animation::Animation(Sprite animationSprite, int startFrame, int animationFrameCount, float playbackRate,
	bool shouldLoop)
	: sprite(std::move(animationSprite)),
	  framesPerSecond(std::max(0.0f, playbackRate)),
	  looping(shouldLoop) {
	const int totalFrames = std::max(1, this->sprite.getFrameCount());
	firstFrame = std::clamp(startFrame, 0, totalFrames - 1);
	frameCount = std::clamp(animationFrameCount, 1, totalFrames - firstFrame);
}

void Animation::update(float deltaTime) {
	if (finished || deltaTime <= 0.0f || framesPerSecond <= 0.0f) return;

	elapsed += deltaTime;
	const float duration = static_cast<float>(frameCount) / framesPerSecond;
	if (looping) {
		if (elapsed >= duration) elapsed = std::fmod(elapsed, duration);
	} else if (elapsed >= duration) {
		elapsed = duration;
		finished = true;
	}
}

void Animation::reset() {
	elapsed = 0.0f;
	finished = false;
}

int Animation::getCurrentFrame() const {
	if (framesPerSecond <= 0.0f) return firstFrame;

	const int offset = std::clamp(
		static_cast<int>(elapsed * framesPerSecond),
		0,
		frameCount - 1
	);
	return firstFrame + offset;
}

bool Animation::isFinished() const {
	return finished;
}

Sprite& Animation::getSprite() {
	return sprite;
}

const Sprite& Animation::getSprite() const {
	return sprite;
}
