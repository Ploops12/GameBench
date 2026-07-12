#pragma once

#include <vector>
#include "Sprite.hxx"

class Animation {
public:
	Animation();

	void setFrames(Texture2D texture, const std::vector<Rectangle>& frameRects, float secondsPerFrame, bool looped = true);
	void update(float dt);
	void reset();
	const Sprite& currentFrame() const;

private:
	std::vector<Sprite> frames;
	float frameDuration;
	float timer;
	int currentIndex;
	bool loop;
};
