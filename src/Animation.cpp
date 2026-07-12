#include "Animation.hxx"
#include <algorithm>

Animation::Animation(const char* path, int columns, int rows, int requestedFrameCount,
	float framesPerSecond, bool looping) :
	sprite(path, columns, rows),
	frameCount(std::clamp(requestedFrameCount, 1, columns * rows)),
	framesPerSecond(std::max(0.01f, framesPerSecond)), looping(looping) {}

void Animation::update(float dt) {
	if (finished || !sprite.isReady()) return;
	accumulator += dt;
	const float frameDuration = 1.0f / framesPerSecond;
	while (accumulator >= frameDuration) {
		accumulator -= frameDuration;
		int next = sprite.getFrame() + 1;
		if (next >= frameCount) {
			if (looping) next = 0;
			else {
				next = frameCount - 1;
				finished = true;
			}
		}
		sprite.setFrame(next);
	}
}

void Animation::reset() {
	accumulator = 0.0f;
	finished = false;
	sprite.setFrame(0);
}

void Animation::draw(Vector2 position, float scale, Color tint) const {
	sprite.draw(position, scale, tint);
}

void Animation::drawBillboard(const Camera3D& camera, Vector3 position, float size, Color tint) const {
	sprite.drawBillboard(camera, position, size, tint);
}
