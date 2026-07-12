#include "Animation.hxx"

Animation::Animation(Sprite* sprite, int columns, int rows, float fps, int row)
	: sprite(sprite), columns(columns), rows(rows), row(row), fps(fps) {
}

void Animation::setSprite(Sprite* value) {
	sprite = value;
}

void Animation::setGrid(int valueColumns, int valueRows) {
	columns = valueColumns;
	rows = valueRows;
}

void Animation::setPlayback(float valueFps, int valueStartFrame, int valueFrameCount, int valueRow) {
	fps = valueFps;
	startFrame = valueStartFrame;
	frameCount = valueFrameCount > 0 ? valueFrameCount : 1;
	row = valueRow;
	currentFrameIndex = 0;
	accumulator = 0.0f;
}

void Animation::update(float dt) {
	if (!sprite || fps <= 0.0f || frameCount <= 1) {
		return;
	}

	accumulator += dt;
	const float frameDuration = 1.0f / fps;
	while (accumulator >= frameDuration) {
		accumulator -= frameDuration;
		currentFrameIndex = (currentFrameIndex + 1) % frameCount;
	}
}

bool Animation::isReady() const {
	return sprite && sprite->isLoaded();
}

Rectangle Animation::currentFrame() const {
	if (!sprite || !sprite->isLoaded()) {
		return Rectangle {0, 0, 0, 0};
	}

	return sprite->frame(columns, rows, row * columns + startFrame + currentFrameIndex);
}

const Texture2D& Animation::texture() const {
	return sprite->getTexture();
}
