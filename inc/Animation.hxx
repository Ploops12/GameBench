#pragma once

#include "Sprite.hxx"

class Animation {
public:
	Animation() = default;
	Animation(Sprite* sprite, int columns, int rows, float fps, int row = 0);

	void setSprite(Sprite* value);
	void setGrid(int columns, int rows);
	void setPlayback(float fps, int startFrame, int frameCount, int row = 0);
	void update(float dt);

	bool isReady() const;
	Rectangle currentFrame() const;
	const Texture2D& texture() const;
private:
	Sprite* sprite {nullptr};
	int columns {1};
	int rows {1};
	int row {0};
	int startFrame {0};
	int frameCount {1};
	int currentFrameIndex {0};
	float fps {0.0f};
	float accumulator {0.0f};
};
