#pragma once

class Animation {
public:
	void update(float dt, float frameTime = 0.12f) { timer += dt; if (timer >= frameTime) { timer = 0.0f; frame = (frame + 1) % frameCount; } }
	int getFrame() const { return frame; }
	void setFrameCount(int count) { frameCount = count > 0 ? count : 1; }

private:
	float timer{0.0f};
	int frame{0};
	int frameCount{1};
};
