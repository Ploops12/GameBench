#pragma once

#include "Sprite.hxx"

class Animation {
public:
	Animation() = default;
	Animation(const char* path, int columns, int rows, int frameCount, float framesPerSecond, bool looping = true);

	void update(float dt);
	void reset();
	void draw(Vector2 position, float scale = 1.0f, Color tint = WHITE) const;
	void drawBillboard(const Camera3D& camera, Vector3 position, float size, Color tint = WHITE) const;
	bool isFinished() const { return finished; }
	Sprite& getSprite() { return sprite; }
	const Sprite& getSprite() const { return sprite; }

private:
	Sprite sprite;
	int frameCount {1};
	float framesPerSecond {1.0f};
	float accumulator {};
	bool looping {true};
	bool finished {};
};
