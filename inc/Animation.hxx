#pragma once

#include "Sprite.hxx"

class Animation {
public:
	Animation() = default;
	Animation(Sprite sprite, float framesPerSecond, bool looping = true);

	void update(float dt);
	void reset();
	void setPlaying(bool playing);
	[[nodiscard]] bool isPlaying() const;
	[[nodiscard]] bool isFinished() const;
	[[nodiscard]] Sprite& sprite();
	[[nodiscard]] const Sprite& sprite() const;

private:
	Sprite sprite_;
	float framesPerSecond_ {8.0f};
	float accumulator_ {0.0f};
	bool looping_ {true};
	bool playing_ {true};
	bool finished_ {false};
};
