#pragma once

#include "Sprite.hxx"

class Animation {
public:
	Animation() = default;
	Animation(const Sprite* sprite, int frameCount, float framesPerSecond);

	void setSprite(const Sprite* newSprite);
	void configure(int frameCount, float framesPerSecond);
	void update(float deltaTime);
	void reset();
	int getFrame() const;
	Rectangle getSourceRectangle() const;
	void draw(Rectangle destination, Color tint = WHITE) const;

private:
	const Sprite* sprite {nullptr};
	int frameCount {1};
	int frame {0};
	float framesPerSecond {1.0f};
	float elapsed {0.0f};
};
