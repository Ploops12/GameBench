#include "Animation.hxx"

#include <algorithm>
#include <utility>

Animation::Animation(Sprite sprite, float framesPerSecond, bool looping)
	: sprite_(std::move(sprite)), framesPerSecond_(std::max(0.1f, framesPerSecond)), looping_(looping) {
}

void Animation::update(float dt) {
	if (!playing_ || finished_ || sprite_.frameCount() <= 1) {
		return;
	}
	accumulator_ += dt;
	const float frameDuration = 1.0f / framesPerSecond_;
	while (accumulator_ >= frameDuration) {
		accumulator_ -= frameDuration;
		const int next = sprite_.frame() + 1;
		if (next >= sprite_.frameCount()) {
			if (looping_) {
				sprite_.setFrame(0);
			} else {
				sprite_.setFrame(sprite_.frameCount() - 1);
				finished_ = true;
				playing_ = false;
			}
		} else {
			sprite_.setFrame(next);
		}
	}
}

void Animation::reset() {
	sprite_.setFrame(0);
	accumulator_ = 0.0f;
	finished_ = false;
	playing_ = true;
}

void Animation::setPlaying(bool playing) {
	playing_ = playing;
}

bool Animation::isPlaying() const {
	return playing_;
}

bool Animation::isFinished() const {
	return finished_;
}

Sprite& Animation::sprite() {
	return sprite_;
}

const Sprite& Animation::sprite() const {
	return sprite_;
}
