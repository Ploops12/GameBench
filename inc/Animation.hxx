#pragma once

class Animation {
public:
	explicit Animation(int frameCount = 1, float frameDuration = 0.1f);

	void reset();
	void update(float deltaTime);
	int frame() const;
	bool finished() const;

private:
	int frameCount{1};
	int currentFrame{0};
	float frameDuration{0.1f};
	float elapsed{0.0f};
};
