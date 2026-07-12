#include "Animation.hxx"

Animation::Animation() : frameDuration(0.1f), timer(0.0f), currentIndex(0), loop(true) {}

void Animation::setFrames(Texture2D texture, const std::vector<Rectangle>& frameRects, float secondsPerFrame, bool looped) {
	frames.clear();
	for (const Rectangle& rect : frameRects) {
		frames.emplace_back(texture, rect);
	}
	frameDuration = secondsPerFrame;
	timer = 0.0f;
	currentIndex = 0;
	loop = looped;
}

void Animation::update(float dt) {
	if (frames.size() < 2 || frameDuration <= 0.0f) {
		return;
	}

	timer += dt;
	while (timer >= frameDuration) {
		timer -= frameDuration;
		if (currentIndex + 1 < static_cast<int>(frames.size())) {
			++currentIndex;
		} else if (loop) {
			currentIndex = 0;
		}
	}
}

void Animation::reset() {
	timer = 0.0f;
	currentIndex = 0;
}

const Sprite& Animation::currentFrame() const {
	return frames[currentIndex];
}
