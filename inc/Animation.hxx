#pragma once

// Sprite-sheet animation helper with fixed frame timing.

#include "Sprite.hxx"

class Animation {
public:
	Animation() = default;
	Animation(const char* path, int frameCount, float secondsPerFrame);

	bool load(const char* path, int frameCount, float secondsPerFrame);
	void update(float dt);
	void reset();
	void draw(Rectangle destination, Color tint = WHITE) const;
	bool isLoaded() const;
	int currentFrame() const;
	void setFrameCountForTest(int frameCount);

private:
	Sprite sprite{};
	float frameTime{0.1f};
	float elapsed{};
	int frame{};
	int testFrameTotal{};
};
