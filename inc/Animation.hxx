#pragma once

#include "Sprite.hxx"

class Animation {
public:
	void SetFrameCount(int count) { frameCount = count > 0 ? count : 1; }
	void SetFrameTime(float seconds) { frameTime = seconds > 0.0f ? seconds : 0.1f; }
	void Reset() { frame = 0; timer = 0.0f; }
	void Update(float dt) {
		timer += dt;
		while (timer >= frameTime) {
			timer -= frameTime;
			frame = (frame + 1) % frameCount;
		}
	}
	int GetFrame() const { return frame; }

private:
	int frame{0};
	int frameCount{1};
	float frameTime{0.1f};
	float timer{0.0f};
};
