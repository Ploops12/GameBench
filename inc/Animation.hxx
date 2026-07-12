#pragma once

#include "Sprite.hxx"

class Animation {
	// Owns a sheet and advances a contiguous frame range at a fixed rate.
public:
	Animation() = default;
	Animation(Sprite sprite, int firstFrame, int frameCount, float framesPerSecond, bool looping = true);

	void update(float deltaTime);
	void reset();
	int getCurrentFrame() const;
	bool isFinished() const;
	Sprite& getSprite();
	const Sprite& getSprite() const;

private:
	Sprite sprite;
	int firstFrame {0};
	int frameCount {1};
	float framesPerSecond {1.0f};
	bool looping {true};
	float elapsed {0.0f};
	bool finished {false};
};
